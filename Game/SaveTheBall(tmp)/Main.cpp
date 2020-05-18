# include <Siv3D.hpp> // OpenSiv3D v0.4.1

//メイン関数
void Main()
{
	while (System::Update())
	{

	}
}

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
	struct Balls//各球を表す構造体
	{
		double x;//現在のx座標
		double y;//現在のy座標
		double radius;//半径
		double moveByX;//1フレーム当たりのx座標の増分
		double moveByY;//1フレーム当たりのy座標の増分
		Circle circle;
		double distance(Balls ball1, Balls ball2) {//与えられた2つのボールの距離を返す関数
			return std::sqrt((ball1.x - ball2.x) * (ball1.x - ball2.x) + (ball1.y - ball2.y) * (ball1.y - ball2.y));
		}
	};
	

};