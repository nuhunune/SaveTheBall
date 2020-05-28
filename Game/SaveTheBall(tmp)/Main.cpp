# include <Siv3D.hpp> // OpenSiv3D v0.4.1
static constexpr int windowWidth = 1200;
static constexpr int windowHeight = 900;
int score;//ゲームの点数


//各シーンを表す列挙型
enum Scenes {
	PLAYING_GAME//ゲーム中
	,TITLE//タイトル
	,RESULT//結果表示(点数など)
	,LANKING//結果表示(順位)
	,GAMEOVER//HPが0になったとき
	,TIMEOVER//残り時間が0になったとき

};
//現在のシーン
Scenes scene;

//タイトル表示用クラス
class Title {

};

//結果表示用クラス
class Result {
	const Font font200 = Font(200);
public:
	void gameOver() {
		font200(U"Game Over!").draw(Arg::center = Vec2(600,450));
	}
	void timeUp() {
		font200(U"Time Up!").draw(Arg::center = Vec2(600, 450));
	}
};

//ゲームの本体クラス
class Game {
	static constexpr int ballSize = 70;//ボールの大きさ(すべて同じ)
	static constexpr double speedOfBalls = 4;//1フレーム当たりのボールの移動距離(すべて同じ)
	static constexpr int rightEdgeOfMainScreen=1200, leftEdgeOfMainScreen=200, topEdgeOfMainScreen=0, bottomEdgeOfMainScreen=750;//ゲーム本体画面の範囲
	static constexpr int safeAreaSize = 85;//安全エリアの大きさ
	static constexpr int okAreaSize = 100;//大丈夫エリアの大きさ
	static constexpr int maxHP = 1000;//HPの最大値
	static constexpr int maxNumberOfBall = 9;//ボールの最大数 レベルの最大値でもある
	static constexpr int timeLimit = 30;//制限時間の秒数
	int restHP;
	const Font font100 = Font(100);//フォント
	Stopwatch stopwatch;


	struct ComplexNumber {//複素数を扱う構造体
		//扱う複素数はiを虚数単位としてa+biとする
		double a,b;
		ComplexNumber(double a, double b) {//実部と虚部を引数にとるコンストラクタ
			this->a = a;
			this->b = b;
		}
		double abs() {//絶対値
			return std::sqrt(a * a + b * b);
		}
		ComplexNumber unitVector() {//偏角が引数と等しく絶対値が1の複素数を返す
			double abs = (*this).abs();
			return ComplexNumber((*this).a / abs, (*this).b / abs);
		}
		ComplexNumber operator+(ComplexNumber rightOperand) {//加算演算子+
			return ComplexNumber(a + rightOperand.a, b + rightOperand.b);
		}
		ComplexNumber operator-(ComplexNumber rightOperand) {//減算演算子-
			return ComplexNumber(a - rightOperand.a, b - rightOperand.b);
		}
		ComplexNumber operator*(ComplexNumber rightOperand) {//乗算演算子*
			double a2 = rightOperand.a;
			double b2 = rightOperand.b;
			return ComplexNumber(a * a2 - b * b2, a * b2 + a2 * b);
		}
		ComplexNumber operator*(double rightOperand) {//複素数の定数倍
			return ComplexNumber(a * rightOperand, b * rightOperand);
		}
		ComplexNumber operator/(ComplexNumber rightOperand) {//割り算(arg1/arg2)
			double absOfRightOperand = rightOperand.abs();
			ComplexNumber reciprocalOfRightOperand = ComplexNumber(rightOperand.a / (absOfRightOperand * absOfRightOperand), -rightOperand.b / (absOfRightOperand * absOfRightOperand));//右オペランドの逆数
			return (*this)*reciprocalOfRightOperand;//逆数との積を返す
		}
		
		String show() {//自分自身を(a+bi)のフォーマットの文字列で返す
			String stringA = ToString(a);
			String stringB = ToString(b);
			return U"(" + stringA + U"+" + stringB + U"i)";
		}
		



	};
	struct Ball//各球を扱う構造体
	{
		double x;//現在のx座標
		double y;//現在のy座標
		double moveByX;//1フレーム当たりのx座標の増分
		double moveByY;//1フレーム当たりのy座標の増分
		static double distance(Ball* ball1, Ball* ball2) {//与えられた2つのボールの距離を返す関数
			return std::sqrt(((*ball1).x - (*ball2).x) * ((*ball1).x - (*ball2).x) + ((*ball1).y - (*ball2).y) * ((*ball1).y - (*ball2).y));
		}
		void changeDirection(double newMoveByX,double newMoveByY) {//1フレーム当たりの増分の変更
			moveByX = newMoveByX;
			moveByY = newMoveByY;
		}
		
		static void changeTheDirectionOfBumpedBalls(Ball* ball1, Ball* ball2) {//衝突した2つのボールの方向を変更　引数の2つのボールがぶつかっている前提
			//物理学的におかしいから余裕があったら修正
			double x1 = (*ball1).x;//それぞれの玉の座標
			double y1 = (*ball1).y;
			double x2 = (*ball2).x;
			double y2 = (*ball2).y;
			double moveByX1 = (*ball1).moveByX;//それぞれの円の1フレーム当たりの座標の増分
			double moveByY1 = (*ball1).moveByY;
			double moveByX2 = (*ball2).moveByX;
			double moveByY2 = (*ball2).moveByY;
			ComplexNumber tangentLine=ComplexNumber(x1-x2,y1-y2)*ComplexNumber(0,1);//2つの円の接線と同じ偏角の複素数
			ComplexNumber directionOfBall1 = ComplexNumber(moveByX1,moveByY1);//2つの円の進行方向を表す複素数
			ComplexNumber directionOfBall2 = ComplexNumber(moveByX2, moveByY2);
			ComplexNumber newDirectionOfBall1 = (tangentLine * tangentLine / directionOfBall1).unitVector()*speedOfBalls;
			ComplexNumber newDirectionOfBall2 = (tangentLine * tangentLine / directionOfBall2).unitVector()*speedOfBalls;
			(*ball1).moveByX = newDirectionOfBall1.a;
			(*ball1).moveByY = newDirectionOfBall1.b;
			(*ball2).moveByX = newDirectionOfBall2.a;
			(*ball2).moveByY = newDirectionOfBall2.b;
		}
		

	
		Ball(double x,double y,double moveByX,double moveByY) {
			this->x = x;
			this->y = y;
			this->moveByX = moveByX;
			this->moveByY = moveByY;
		}
	};
	void addBall() {//ボールの追加
		Circle tmpCircle;//一時的に、他の円と交差するか調べるために作る円
		int32 randomX;//ランダム生成した座標を格納する変数
		int32 randomY;
		bool ok;//どの円にも潜り込まないことを確かめる
		while (true)
		{
			randomX = Random<int32>(leftEdgeOfMainScreen + ballSize, rightEdgeOfMainScreen - ballSize);//座標をランダム生成
			randomY = Random<int32>(topEdgeOfMainScreen + ballSize, bottomEdgeOfMainScreen - ballSize);
			ok = true;
			for (int i = 0; i < balls.size(); i++) {
				if (std::sqrt((balls[i].x - randomX) * (balls[i].x - randomX) + (balls[i].y - randomY) * (balls[i].y - randomY)) < ballSize * 2 + 10) {
					ok = false;
				}
			}
			if (ok) {
				balls << Ball(randomX, randomY, 0, 4);
				break;
			}
		}

	}
	void updateBall() {//ボールの位置の更新と描画
		const int32 numberOfBalls = balls.size();//処理するボールの個数
		for (int i = 0; i < numberOfBalls; i++) {//画面端への衝突時の方向変更
			if (balls[i].x <= leftEdgeOfMainScreen + ballSize) {//左端に衝突するとき
				balls[i].changeDirection(-balls[i].moveByX, balls[i].moveByY);//x軸方向の速さをマイナス一倍
			}
			if (balls[i].x >= rightEdgeOfMainScreen - ballSize) {//右端に衝突するとき
				balls[i].changeDirection(-balls[i].moveByX, balls[i].moveByY);//x軸方向の速さをマイナス一倍
			}
			if (balls[i].y <= topEdgeOfMainScreen + ballSize) {//上端に衝突するとき
				balls[i].changeDirection(balls[i].moveByX, -balls[i].moveByY);//y軸方向の速さをマイナス一倍
			}
			if (balls[i].y >= bottomEdgeOfMainScreen - ballSize) {
				balls[i].changeDirection(balls[i].moveByX, -balls[i].moveByY);//y軸方向の速さをマイナス一倍
			}
		}
		for (int i = 0; i < numberOfBalls; i++) {//ボール同士の衝突時の方向変更
			for (int j = i + 1; j < numberOfBalls; j++) {//同じ衝突を二度見ないようにする
				if (Ball::distance(&balls[i], &balls[j]) <= ballSize * 2) {
					Ball::changeTheDirectionOfBumpedBalls(&balls[i], &balls[j]);
				}
			}
		}
		for (int i = 0; i < numberOfBalls; i++) {//ボール位置の変更
			balls[i].x = balls[i].x + balls[i].moveByX;
			balls[i].y = balls[i].y + balls[i].moveByY;
		}
		//ボールの描画
		for (int i = 0; i < numberOfBalls; i++) {
			if (i == 0) {
				Circle(balls[i].x, balls[i].y, ballSize).draw(Palette::Yellowgreen);
			}
			else {
				Circle(balls[i].x, balls[i].y, ballSize).draw(Palette::White);
			}
			
		}
	}
	
public:
	Array<Ball> balls;//ボール群　0番目が自分が囲うボール
	void init() {//ゲームの初期化
		restHP = maxHP;
		score = 0;
		stopwatch = Stopwatch(true);
		balls << Ball(800, 450, 2, 3);
	}
	void update(){//ゲームの更新
		Rect(leftEdgeOfMainScreen, topEdgeOfMainScreen, rightEdgeOfMainScreen - leftEdgeOfMainScreen, bottomEdgeOfMainScreen - topEdgeOfMainScreen).draw(Color(30,30,30));//ゲーム範囲を黒く塗る
		Point cursor = Cursor::Pos();//カーソル位置
		double restTimeFloat = timeLimit - stopwatch.sF();//浮動小数点での残り時間
		double restTimeInteger = timeLimit - stopwatch.s64();
		//各エリア(自機)の描画
		Circle okArea(cursor, okAreaSize);
		Circle safeArea(cursor, safeAreaSize);
		okArea.draw(Palette::Yellow).drawFrame(0, 10, Palette::Red);
		safeArea.draw(Palette::Skyblue);

		//クリックされたらボールを追加
		if (MouseL.down()&&balls.size()<maxNumberOfBall) {
			addBall();
		}
		//ボールの更新
		updateBall();

		//判定
		Circle ballToSurround(balls[0].x, balls[0].y, ballSize);
		if (safeArea.contains(ballToSurround)) {//安全エリア内にあるとき
			restHP+=3;//HPを3増やす
			score += balls.size() * 3;//スコアをレベル数の三倍増やす
		}
		else if (!okArea.contains(ballToSurround)) {//大丈夫エリア外の時
			restHP--;//HPを1減らす
		}
		else {//ちょうど大丈夫エリアにいるとき
			score += balls.size();//スコアをレベル数増やす
		}
		if (restHP == 0) {//ライフが0なら
			Scene::SetBackground(Palette::Black);
			scene = GAMEOVER;//シーンチェンジ
			return;
		}
		else if (restTimeInteger == 0) {
			Scene::SetBackground(Palette::Black);
			scene = TIMEOVER;
			return;
		}
		if (restHP > maxHP) {
			restHP = maxHP;
		}
		//スコアの描画
		String scoreText = U"Score:" +ToString(score);
		font100(scoreText).draw(400, 750);
		//残り時間の描画
		font100(ToString(restTimeInteger)).draw(Arg::center=Vec2(100,100));//数字
		//扇型の外周
		if (restTimeFloat < timeLimit / 4.0) {//残り時間が四分の一以下の時
			Circle(100, 100, 70).drawArc(0, (restTimeFloat / timeLimit) * 2 * std::_Pi, 0, 10, Palette::Red);
		}
		else if (restTimeFloat < timeLimit / 2.0) {//四分の一以上二分の一以下の時
			Circle(100, 100, 70).drawArc(0, (restTimeFloat / timeLimit) * 2 * std::_Pi, 0, 10, Palette::Orange);
		}
		else {//それより多いとき
			Circle(100, 100, 70).drawArc(0, (restTimeFloat / timeLimit) * 2 * std::_Pi, 0, 10, Palette::Yellowgreen);
		}

		//レベルの描画
		font100(U"Lv." + ToString(balls.size())).draw(Arg::topCenter = Vec2(100, 150));
		
		//HPの描画
		font100(U"HP").draw(Arg::topCenter = Vec2(100, 350));//HPという文字
		Rect(Arg::topCenter(100, 500), 100, 350).drawFrame(0,10,Palette::Black);//枠
		double propotionOfRestHP = ((double)restHP) / maxHP;
		if (propotionOfRestHP < 0.25) {//残りHPが25パーセント以下なら
			Rect(Arg::bottomCenter(100, 850), 100, 350 * propotionOfRestHP).draw(Palette::Red);//赤く
		}
		else if (propotionOfRestHP < 0.5) {
			Rect(Arg::bottomCenter(100, 850), 100, 350 * propotionOfRestHP).draw(Palette::Orange);
		}
		else if (propotionOfRestHP < 0.75) {
			Rect(Arg::bottomCenter(100, 850), 100, 350 * propotionOfRestHP).draw(Palette::Yellow);
		}
		else {
			Rect(Arg::bottomCenter(100, 850), 100, 350 * propotionOfRestHP).draw(Palette::Blue);
		}
	}
};

//メイン関数
void Main()
{
	Window::Resize(Size(windowWidth,windowHeight));//windowのサイズは1200x900にする(あとで上の方に定数用意するかも)
	Game game;//ゲーム本体の本体
	Result result;//結果表示用
	game.init();
	while (System::Update())//switch文で分岐しよう
	{
		switch (scene)
		{
		case PLAYING_GAME:
			Scene::SetBackground(Palette::Skyblue);
			game.update();
			break;
		case TITLE:
			break;
		case RESULT:
			break;
		case LANKING:
			break;
		case GAMEOVER:
			result.gameOver();
			break;
		case TIMEOVER:
			result.timeUp();
			break;
		}
	}
}