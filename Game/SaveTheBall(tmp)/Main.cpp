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
	class ComplexNumber {//複素数を扱うクラス
	private:
	public:
		//扱う複素数はiを虚数単位としてa+biとする
		double a,b;
		ComplexNumber(double a, double b) {//実部と虚部を引数にとるコンストラクタ
			this->a = a;
			this->b = b;
		}
		static double abs(ComplexNumber* arg) {//絶対値
			return std::sqrt(((*arg).a) * ((*arg).a)+ ((*arg).b)* ((*arg).b));
		}
		static ComplexNumber unitVector(ComplexNumber* arg) {//偏角が引数と等しく絶対値が1の複素数を返す
			double absOfArg = ComplexNumber::abs(arg);
			return ComplexNumber((*arg).a / absOfArg, (*arg).b / absOfArg);
		}
		static ComplexNumber addition(ComplexNumber* arg1, ComplexNumber* arg2) {//加算(arg1+arg2)
			return ComplexNumber((*arg1).a + (*arg2).a, (*arg1).b + (*arg2).b);
		}
		static ComplexNumber subtraction(ComplexNumber* arg1, ComplexNumber* arg2) {//減算(arg1-arg2)
			return ComplexNumber((*arg1).a - (*arg2).a, (*arg1).b - (*arg2).b);
		}
		static ComplexNumber multiplication(ComplexNumber* arg1, ComplexNumber* arg2) {//乗算(arg1*arg2)
			double a1 = (*arg1).a;//各引数の実部と虚部を変数に格納
			double b1 = (*arg1).b;
			double a2 = (*arg2).a;
			double b2 = (*arg2).b;
			return ComplexNumber(a1*a2-b1*b2,a1*b2+a2*b1);
		}
		static ComplexNumber dividion(ComplexNumber* arg1, ComplexNumber* arg2) {//割り算(arg1/arg2)
			double absOfArg2 = ComplexNumber::abs(arg2);
			ComplexNumber* reciprocalOfArg2 = &ComplexNumber((*arg2).a / (absOfArg2 * absOfArg2), -(*arg2).b / (absOfArg2 * absOfArg2));//arg2の逆数
			return ComplexNumber::multiplication(arg1, reciprocalOfArg2);//逆数との積を返す
		}
		String show() {//自分自身を(a+bi)のフォーマットの文字列で返す
			String stringA = ToString(a);
			String stringB = ToString(b);
			return U"(" + stringA + U"+" + stringB + U"i)";
		}
		



	};
	class Ball//各球を扱うクラス
	{
	private:
		double x;//現在のx座標
		double y;//現在のy座標
		double radius;//半径
		double moveByX;//1フレーム当たりのx座標の増分
		double moveByY;//1フレーム当たりのy座標の増分
		Circle circle;
		double distance(Ball* ball1, Ball* ball2) {//与えられた2つのボールの距離を返す関数
			return std::sqrt(((*ball1).x - (*ball2).x) * ((*ball1).x - (*ball2).x) + ((*ball1).y - (*ball2).y) * ((*ball1).y - (*ball2).y));
		}
		void changeDirection(Ball* ball,double newMoveByX,double newMoveByY) {//1フレーム当たりの増分の変更
			(*ball).moveByX = newMoveByX;
			(*ball).moveByY = newMoveByY;
		}
		
		void changeTheDirectionOfBumpedBalls(Ball* ball1, Ball* ball2) {//衝突した2つのボールの方向を変更　引数の2つのボールがぶつかっている前提
			//複素数を実装してからやる
		}

	public:
		void update() {
			/*
			最初に壁の反射
			次にfor回してボール同士の反射
			↑moveByを変えるだけ
			次にボールの位置を変える
			描画をいつやるかは未定
			
			*/
		}
		
	};
public:
	Array<Ball> balls;
	//static void cNumTest(double a1, double b1, double a2, double b2) {//複素数クラスのテスト用
	//	ComplexNumber c1(a1, b1);
	//	ComplexNumber c2(a2, b2);
	//	Print << c1.show() << U"+" << c2.show() << U"=" << ComplexNumber::addition(&c1, &c2).show();
	//	Print << c1.show() << U"-" << c2.show() << U"=" << ComplexNumber::subtraction(&c1, &c2).show();
	//	Print << c1.show() << U"*" << c2.show() << U"=" << ComplexNumber::multiplication(&c1, &c2).show();
	//	Print << c1.show() << U"/" << c2.show() << U"=" << ComplexNumber::dividion(&c1, &c2).show();
	//}
	

};

//メイン関数
void Main()
{

	while (System::Update())
	{

	}
}