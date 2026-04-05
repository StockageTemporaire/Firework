#include <stdio.h>
#include <raylib.h>
#include <math.h>
//宏定义区
#define WIDTH 1000
#define HEIGHT 900
#define MAX_ROCKETS    20   // 同时存在的最大火箭数量
#define MAX_EXPLOSIONS 30   // 同时存在的最大爆炸数量
//数据类型定义区
typedef struct
{
	Vector2 pos;
	int id;
	float speed;
	float stop;//停止时的坐标
	bool IsActive;
	bool IsFlicker;//是否闪烁
}Rise;
typedef struct
{
	Vector2 pos;
	Vector2 CenterPos;
	int id;
	bool IsActive;
	float r;
	float max_r;
}Explode;
//全局变量声明区
Texture2D risetex;
int FrameCount = 0;//当前帧数
Color* ColorList[5];
Rise rockets[MAX_ROCKETS];
Explode explosions[MAX_EXPLOSIONS];
//函数声明区
void rise_init(Rise* rise);
void rise_update(Rise* rise);
void rise_draw(Rise* rise);
void draw_risetex(Texture2D tex, float x, float y, int id, int width, int height);
void explode_init(Explode* explode, Rise* rise);
void explode_update(Explode* explode);
void explode_draw(Explode* explode);
//主函数
int main()
{
	InitWindow(1000, 900, "firework");
	SetTargetFPS(90);
	InitAudioDevice();
	Music music = LoadMusicStream("SmallLucky.mp3");
	Image riseimg = LoadImage("rise.png");
	Image explodeimg = LoadImage("explode.png");
	risetex = LoadTextureFromImage(riseimg);
	for (int i = 0; i < 5; i++) {
		Image tile = ImageFromImage(explodeimg, (Rectangle) { 240 * i, 0.0, 240, 230 });
		ColorList[i] = LoadImageColors(tile);
		UnloadImage(tile);
	}
	PlayMusicStream(music);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < MAX_ROCKETS; j++) {
			if (!rockets[j].IsActive) {// 找到空闲槽位
				rise_init(&rockets[j]);
				break;
			}
		}
	}
	while (!WindowShouldClose()) {
		UpdateMusicStream(music);
		static int spawnTimer = 0;
		spawnTimer++;
		if (spawnTimer >= 90) {// 每90帧生成一个新火箭
			spawnTimer = 0;
			for (int i = 0; i < MAX_ROCKETS; i++) {
				if (!rockets[i].IsActive) {
					rise_init(&rockets[i]);
					break;
				}
			}
		}
		BeginDrawing();
		ClearBackground(BLACK);
		for (int i = 0; i < MAX_ROCKETS; i++) {
			if (rockets[i].IsActive) {
				rise_draw(&rockets[i]);
			}
		}
		for (int i = 0; i < MAX_EXPLOSIONS; i++) {
			if (explosions[i].IsActive) {
				explode_draw(&explosions[i]);
			}
		}
		EndDrawing();
		for (int i = 0; i < MAX_ROCKETS; i++) {
			if (rockets[i].IsActive) {
				rise_update(&rockets[i]);
			}
		}
		for (int i = 0; i < MAX_EXPLOSIONS; i++) {
			if (explosions[i].IsActive) {
				explode_update(&explosions[i]);
			}
		}
		FrameCount++;
	}
	UnloadMusicStream(music);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
//函数定义区
void rise_init(Rise* rise)
{
	rise->pos.x = GetRandomValue(0, WIDTH);
	rise->pos.y = HEIGHT - 50;
	rise->id = GetRandomValue(0, 4);
	rise->IsActive = true;
	rise->IsFlicker = true;
	rise->speed = GetRandomValue(2, 6);
	rise->stop = GetRandomValue(0, HEIGHT / 2);

}
void rise_update(Rise* rise)
{
	if (!rise->IsActive) return;
	rise->pos.y -= rise->speed;
	if (FrameCount % 10 == 0) {
		rise->IsFlicker = !rise->IsFlicker;
	}
	if (rise->pos.y <= rise->stop) {
		rise->IsActive = false;
		for (int i = 0; i < MAX_EXPLOSIONS; i++) {
			if (!explosions[i].IsActive) {
				explode_init(&explosions[i], rise);
				break;
			}
		}
	}
}
void rise_draw(Rise* rise)
{
	if (!rise->IsActive) return;
	draw_risetex(risetex, rise->pos.x, rise->pos.y, (rise->IsFlicker ? rise->id + 5 : rise->id), 20, 48);
}
void draw_risetex(Texture2D tex, float x, float y, int id, int width, int height)
{
	DrawTexturePro(tex, (Rectangle) { 20 * id, 0.0, width, height }, (Rectangle) { x, y, width, height }, (Vector2){0.0, 0.0}, 0.0, WHITE);
}
void explode_init(Explode* explode, Rise* rise)
{
	explode->pos.x = rise->pos.x;
	explode->pos.y = rise->pos.y;
	explode->IsActive = true;
	explode->id = rise->id;
	explode->r = 0.5f;
	explode->max_r = 120;
	explode->CenterPos = (Vector2){ 120, 115 };
}
void explode_update(Explode* explode)
{
	if (!explode->IsActive) return;
	explode->r += 1.0f;
	if (explode->r >= explode->max_r) {
		explode->IsActive = false;
	}
}
void explode_draw(Explode* explode)
{
	if (!explode->IsActive) return;
	int thickness = 120;  // 环的厚度

	for (double a = 0; a < 2 * PI; a += 0.01) {
		for (int offset = -thickness; offset <= 0; offset++) {
			float currentR = explode->r + offset;
			if (currentR < 0) continue;
			int texX = cos(a) * currentR + explode->CenterPos.x;
			int texY = sin(a) * currentR + explode->CenterPos.y;
			if (texX >= 0 && texX < 240 && texY >= 0 && texY < 230) {
				int screenX = cos(a) * currentR + explode->pos.x;
				int screenY = sin(a) * currentR + explode->pos.y;
				DrawPixel(screenX, screenY, ColorList[explode->id][texY * 240 + texX]);
			}
		}
	}
}
