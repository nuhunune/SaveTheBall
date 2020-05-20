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
	static constexpr int rightEdgeOfMainScreen=1200, leftEdgeOfMainScreen=0, topEdgeOfMainScreen=0, bottomEdgeOfMainScreen=900;

	class ComplexNumber {//複素数を扱うクラス
	private:
	public:
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
	class Ball//各球を扱うクラス
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
			//複素数を実装してからやる
		}

	public:
		Ball(double x,double y,double moveByX,double moveByY) {
			this->x = x;
			this->y = y;
			this->moveByX = moveByX;
			this->moveByY = moveByY;
		}
		void update() {
			/*
			最初に壁の反射
			次にfor回してボール同士の反射
			↑moveByを変えるだけ
			次にボールの位置を変える
			描画をいつやるかは未定
			注：二つ以上の衝突が同じボールに対して同時に起こると多分バグる
			*/
			const int numberOfBalls = (int)balls.size();//処理するボールの個数
			for (int i = 0; i < numberOfBalls; i++) {//画面端への衝突時の方向変更
				if (balls[i].x <= leftEdgeOfMainScreen+ballSize) {//左端に衝突するとき
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

		}
		
	};
public:
	static Array<Ball> balls;
	//static void cNumTest(double a1, double b1, double a2, double b2) {//複素数クラスのテスト用
	//	ComplexNumber c1(a1, b1);
	//	ComplexNumber c2(a2, b2);
	//	Print << c1.show() << U"+" << c2.show() << U"=" << (c1+c2).show();
	//	Print << c1.show() << U"-" << c2.show() << U"=" << (c1-c2).show();
	//	Print << c1.show() << U"*" << c2.show() << U"=" << (c1*c2).show();
	//	Print << c1.show() << U"/" << c2.show() << U"=" << (c1/c2).show();
	//}
	static void addBall() {//ballのテスト用
		balls << Ball(600, 450, 30, 30);
	}
	

};

//メイン関数
void Main()
{
	Window::Resize(Size(1200,900));
	while (System::Update())
	{

	}
}