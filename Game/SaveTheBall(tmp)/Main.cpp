# include <Siv3D.hpp> // OpenSiv3D v0.4.1

//各シーンを表す列挙型
enum Scenes {
	PLAYING_GAME//ゲーム中
	,TITLE//タイトル
	,RESULT//結果表示(点数など)
	,LANKING//結果表示(順位)
	

};
//現在のシーン
Scenes scene;

//タイトル表示用クラス
class Title {

};

//結果表示用クラス
class Result {

};

//ゲームの本体クラス
class Game {
	static constexpr int ballSize = 100;//ボールの大きさ(すべて同じ)
	static constexpr double speedOfBalls = 4;//1フレーム当たりのボールの移動距離(すべて同じ)
	static constexpr int rightEdgeOfMainScreen=1200, leftEdgeOfMainScreen=0, topEdgeOfMainScreen=0, bottomEdgeOfMainScreen=900;//ゲーム本体画面の範囲
	static constexpr int safeAreaSize = 115;//安全エリアの大きさ
	static constexpr int okAreaSize = 130;//大丈夫エリアの大きさ
	static constexpr int maxHP = 1000;//HPの最大値
	static constexpr int maxNumberOfBall = 10;//ボールの最大数
	int restHP;


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
		Print << U"ok";

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
	void gameOver() {
		//ライフ0になった時の処理
		Print << U"GameOver!";
	}
public:
	Array<Ball> balls;//ボール群　0番目が自分が囲うボール
	//static void cNumTest(double a1, double b1, double a2, double b2) {//複素数クラスのテスト用
	//	ComplexNumber c1(a1, b1);
	//	ComplexNumber c2(a2, b2);
	//	Print << c1.show() << U"+" << c2.show() << U"=" << (c1+c2).show();
	//	Print << c1.show() << U"-" << c2.show() << U"=" << (c1-c2).show();
	//	Print << c1.show() << U"*" << c2.show() << U"=" << (c1*c2).show();
	//	Print << c1.show() << U"/" << c2.show() << U"=" << (c1/c2).show();
	//}
	void init() {//ゲームの初期化
		restHP = maxHP;
		balls << Ball(800, 450, 2, 3);
		balls << Ball(400, 450, -2, 4);
	}
	void update(){//ゲームの更新
		Point cursor = Cursor::Pos();
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
		if (safeArea.contains(ballToSurround)) {
			restHP+=3;
		}
		else if (!okArea.contains(ballToSurround)) {
			restHP--;
		}
		if (restHP == 0/* || !okArea.intersects(ballToSurround)*/) {//自機が完全に外に出たかライフが0なら
			gameOver();
		}
		if (restHP > maxHP) {
			restHP = maxHP;
		}
		Print << restHP;
	}
	void test() {//ボールのテスト用
		balls << Ball(800, 450, 2, 3);
		balls << Ball(400, 450, -2, 4);

	}

};

//メイン関数
void Main()
{
	Window::Resize(Size(1200,900));//windowのサイズは1200x900にする(あとで上の方に定数用意するかも)
	Game game;//ゲーム本体の本体
	game.init();
	while (System::Update())//switch文で分岐しよう
	{
		game.update();
	}
}