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

};