// Gravitational-Lensing.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// -------- 定数 --------

//幾何学単位系 M = 1.0
constexpr double RS = 2.0;				// シュワルツシルト半径
constexpr double PI = 3.14159265358979; // 円周率
constexpr double T_scale = 8000.0;		// BHの温度

// -------- Vec3 構造体 --------

struct Vec3 {
	double x, y, z;
};

// 加算
Vec3 operator+(const Vec3& a, const Vec3& b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

// 減算
Vec3 operator-(const Vec3& a, const Vec3& b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

// 乗算
Vec3 operator*(double s, const Vec3& a)
{
	return { s * a.x, s * a.y, s * a.z };
}

// 除算
Vec3 operator/(const Vec3& a, double s)
{
	return { a.x / s, a.y / s, a.z / s };
}

// 内積
double dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z ;
}

// 外積
Vec3 cross(const Vec3& a, const Vec3& b)
{
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

// 長さ
double length(const Vec3& a)
{
	return sqrt(dot(a,a));
}

// 正規化
Vec3 normalize(const Vec3& a) 
{
	const double len = length(a);

	if (len < 1e-8) return { 0.0, 0.0, 0.0 };

	return a / len;
}

// シュワルツシルト時空での光子の軌道方程式
void rhs(double u, double p, double& du, double& dp)
{
	du = p;							// du/dphi
	dp = -u + 1.5 * RS * u * u;		// dp/dphi
}

// 温度計算
double diskTemperature(double r, double rin) 
{
	double T_disk = T_scale * std::pow((rin / r), 0.75)
		* std::pow((1 - std::sqrt(rin / r)), 0.25);

	return T_disk;
}

// 温度で色を判定
void blackbodycolor(double T, double& red, double& green, double& blue) 
{
	double tmpCalc;
	T = std::clamp(T, 1000.0, 40000.0);
	double tmp_kelvin = T / 100.0;

	// red
	if (tmp_kelvin <= 66) {
		red = 255;
	}
	else {
		tmpCalc = tmp_kelvin - 60;
		tmpCalc = 329.698727446 * std::pow(tmpCalc, -0.1332047592);
		red = tmpCalc;
		if (red < 0) red = 0;
		if (red > 255) red = 255;
	}

	// green
	if (tmp_kelvin <= 66) {
		tmpCalc = tmp_kelvin;
		tmpCalc = 99.4708025861 * std::log(tmpCalc) - 161.1195681661;
		green = tmpCalc;
		if (green < 0) green = 0;
		if (green > 255) green = 255;
	}
	else {
		tmpCalc = tmp_kelvin - 60;
		tmpCalc = 288.1221695283 * std::pow(tmpCalc, -0.0755148492);
		green = tmpCalc;
		if (green < 0) green = 0;
		if (green > 255) green = 255;
	}

	// blue
	if (tmp_kelvin >= 66) {
		blue = 255;
	}
	else if (tmp_kelvin <= 19) {
		blue = 0;
	}
	else{
		tmpCalc = tmp_kelvin - 10;
		tmpCalc = 138.5177312231 * std::log(tmpCalc) - 305.0447927307;
		
		blue = tmpCalc;
		if (blue < 0) blue = 0;
		if (blue > 255) blue = 255;
	}
	red /= 255;
	green /= 255;
	blue /= 255;
}


// 返す情報
struct RayHit {
	int result;		// 0:落ちた, 1:脱出, 2:使い切った 3:降着円盤
	double rHit;	// 円盤にあたった半径
	double aHit;	// 円盤にあたった位置の角度
	Vec3 escDir;	// 逃げた方向
	double bz;		// 光線の角運動量のz成分
};


// -------- camera --------

int main() {
	const double r0 = 40;					// カメラとBH中心の距離
	const double incl = 80;					// カメラの傾き
	const double fovdeg = 60;				// カメラの視野角
	const double radi = incl * PI / 180;	// 傾きをラジアンに変換
	const Vec3 cam = r0 * Vec3{ std::sin(radi), 0, std::cos(radi) };
	const Vec3 fwd = -1 * normalize(cam);	// カメラの正面方向
	const Vec3 right = normalize(cross(fwd, { 0, 0, 1 }));	// カメラの右方向
	const Vec3 up = cross(right, fwd);		// カメラの上方向

	int skyW, skyH, skychannels;
	unsigned char* skydata = stbi_load("sky_Gravity_Lensing.png", &skyW, &skyH, &skychannels, 3);
	if (!skydata) {
		std::cerr << "読み込み失敗: " << stbi_failure_reason() << '\n';
		return 2;
	}
	std::cout << "size: " << skyW << " x " << skyH << '\n';

	// -------- 背景の色をとる --------

	const int W = 520;
	const int H = 520;
	const double aspect = static_cast<double>(W) / H;	// アスペクト比
	const double tanH = std::tan(fovdeg * PI / 180.0 / 2.0);	// 視野角の半分のtan
	const Vec3 camHat = (1.0 / r0) * cam;	// カメラ位置ベクトルを正規化
	const double sqrtF = std::sqrt(1 - RS / r0); // 静止観測者の座標系への補正
	const double uH = 1.0 / RS;				// 地平面の逆半径
	const double uEsc = 1.0 / (900.0 * r0); // 脱出判定に使う逆半径
	const double h0 = 0.012;				// 数値計算の刻み幅
	const int frame = 90;					// フレーム数
	const int maxsteps = 6000;				// 最大の積分回数
	const double rin = 6.0;					// 降着円盤の内側の半径
	const double rout = 18.0;				// 降着円盤の外側の半径
	const double T_max = 0.488 * T_scale;

	std::vector<unsigned char> out(W * H * 3, 0);

	auto Samplesky = [&](const Vec3& d, unsigned char rgb[3]) {

		// 方向ベクトルから緯度・経度を求める
		double skydz = std::clamp(d.z, -1.0, 1.0);
		double lat = std::asin(skydz);
		double lon = std::atan2(d.y, -d.x);

		// 経度・緯度を画像座標へ変換
		const int srcX = std::min(static_cast<int>(((lon + PI) / PI * 0.5 * skyW)), skyW - 1);
		const int srcY = std::min(static_cast<int>(((PI * 0.5 - lat) / PI) * skyH), skyH - 1);

		for (int skych = 0; skych < 3; ++skych) {
			rgb[skych] = skydata[(srcY * skyW + srcX) * 3 + skych];
		}

		};

	auto traceRay = [&](const Vec3& dir) -> RayHit {
		RayHit hit;
		hit.result = 2;

		// 光子の軌道面を張る
		const double cosRa = dot(dir, camHat);
		const double sinRa = std::sqrt(std::max(1.0 - cosRa * cosRa, 1e-14));
		const Vec3 tHat = (1.0 / sinRa) * (dir - cosRa * camHat);

		// 初期条件
		double u = 1.0 / r0;
		double up_ = -cosRa * sqrtF / (r0 * sinRa);
		double b = r0 * sinRa / sqrtF;
		Vec3 n = cross(camHat, tHat);
		hit.bz = -b * n.z;
		double phi = 0.0;

		// -------- RK4で光子の軌道を更新 --------

		for (int step = 0; step < maxsteps; ++step) {
			double r = 1.0 / u;
			const double h = h0 / (1.0 + 6.0 * u * u);
			// 1ステップ前の情報を保存
			double z_Old = r * (std::cos(phi) * camHat.z + std::sin(phi) * tHat.z);
			double r_Old = r;
			double phi_Old = phi;

			// k1
			double k1u, k1p;
			rhs(u, up_, k1u, k1p);

			// k2
			double k2u, k2p;
			rhs(u + 0.5 * h * k1u, up_ + 0.5 * h * k1p, k2u, k2p);

			// k3
			double k3u, k3p;
			rhs(u + 0.5 * h * k2u, up_ + 0.5 * h * k2p, k3u, k3p);

			// k4
			double k4u, k4p;
			rhs(u + h * k3u, up_ + h * k3p, k4u, k4p);

			u += (k1u + 2.0 * k2u + 2.0 * k3u + k4u) * (h / 6.0);
			up_ += (k1p + 2.0 * k2p + 2.0 * k3p + k4p) * (h / 6.0);

			phi += h;

			// 1ステップ更新された情報を保存
			double r_New = 1.0 / u;
			double z_New = r_New * (std::cos(phi) * camHat.z + std::sin(phi) * tHat.z);

			// -------- 降着円盤と光線の交差を判定 --------

			// 事象の地平面に到達した場合
			if (u >= uH) {
				hit.result = 0;
				break;
			}

			// 遠方まで脱出した場合
			if (u <= uEsc) {
				hit.result = 1;
				break;
			}

			// 光線が円盤の平面を横切った場合
			if (z_Old * z_New < 0) {
				double Alpha = z_Old / (z_Old - z_New);
				double r_i = r_Old + (r_New - r_Old) * Alpha;	// 交差した位置の半径を求める
				if (rin <= r_i && r_i <= rout) {
					double phi_hit = phi_Old + (Alpha * h);
					Vec3 hit_Pos = r_i * (std::cos(phi_hit) * camHat + std::sin(phi_hit) * tHat);
					double hit_rad = std::atan2(hit_Pos.y, -hit_Pos.x);
					hit.aHit = hit_rad;
					hit.rHit = r_i;
					hit.result = 3;
					break;
				}
			}
		}

		if (hit.result == 1) {
			double drdphi = -up_ / (u * u);
			double r = 1.0 / u;
			double vR = drdphi * std::cos(phi) - r * std::sin(phi);
			double vT = drdphi * std::sin(phi) + r * std::cos(phi);
			hit.escDir = normalize(vR * camHat + vT * tHat);

		}
		return hit;
		};

	// -------- 画面の各ピクセルから光線を作る --------

	// 光線を計算して保存するループ
	std::vector<RayHit> hits(W * H);
	for (int j = 0; j < H; ++j) {
		for (int i = 0; i < W; ++i) {

			// ピクセル座標からカメラ画面上の位置を作る
			const double px = (2.0 * (i + 0.5) / W - 1.0) * aspect * tanH;
			const double py = (1.0 - 2.0 * (j + 0.5) / H) * tanH;
			const Vec3 dir = normalize(fwd + px * right + py * up);

			hits[j * W + i] = traceRay(dir);
		}
	}

	// フレームのループ
	for (int f = 0; f < frame; ++f) {
		double dt = 1.0;
		double t = f * dt;
		for (int j = 0; j < H; ++j) {
			for (int i = 0; i < W; ++i) {
				const RayHit& hit = hits[j * W + i];
				unsigned char rgb[3];
				switch (hit.result) {

					// ブラックホールに捕獲された場合
				case 0:
					rgb[0] = 0;
					rgb[1] = 0;
					rgb[2] = 0;
					break;

					// 遠方へ脱出した場合
				case 1:
					Samplesky(hit.escDir, rgb);
					break;

					// 最大ステップまでに分類されなかった（デバッグ用）
				case 2:
					rgb[0] = 255;
					rgb[1] = 0;
					rgb[2] = 0;
					break;

					// 降着円盤に到達した
				case 3: {
					double bh_tmp = diskTemperature(hit.rHit, rin);
					double cr, cg, cb;
					blackbodycolor(bh_tmp, cr, cg, cb);
					double bh_light = std::pow((bh_tmp / T_max), 4.0);
					double ang_vel = 1 / std::sqrt(hit.rHit * hit.rHit * hit.rHit);
					int k = 12;
					double stripe = std::sin(k * (hit.aHit - ang_vel * t));
					stripe = (stripe + 1) / 2;
					rgb[0] = static_cast<unsigned char>(255 * cr * bh_light * (0.5 + 0.5 * stripe));
					rgb[1] = static_cast<unsigned char>(255 * cg * bh_light * (0.5 + 0.5 * stripe));
					rgb[2] = static_cast<unsigned char>(255 * cb * bh_light * (0.5 + 0.5 * stripe));
					break;
				}

				default:
					rgb[0] = 255; rgb[1] = 0; rgb[2] = 255;
					break;
				}

				// 出力画像にピクセルを書き込む
				for (int ch = 0; ch < 3; ++ch) {
					out[(j * W + i) * 3 + ch] = rgb[ch];
				}
			}
		}

		// -------- 出力 --------

		std::ostringstream oss;
		oss << "frames/frame_" << std::setw(3) << std::setfill('0') << f << ".png";
		stbi_write_png(oss.str().c_str(), W, H, 3, out.data(), W * 3);
	}

	stbi_image_free(skydata);

	return 0;
}

