// BHGravityLens.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    // Physical constants: SI units
    constexpr double G = 6.67430e-11;       // m^3 / (kg s^2)
    constexpr double c = 299792458.0;      // m / s
    constexpr double solarMassKg = 1.98847e30; // kg
    constexpr double parsecM = 3.085677581491367e16; // m
    constexpr double radToArcsec = 206264.806247;

    // Parameters from the Python code
    const double D_L_pc = 500.0;
    const double D_S_pc = 3000.0;
    const double D_LS_pc = D_S_pc - D_L_pc;
    const double M_solar = 1.0e11;

    // Convert distances to metres and mass to kilograms
    const double D_L = D_L_pc * parsecM;
    const double D_S = D_S_pc * parsecM;
    const double D_LS = D_LS_pc * parsecM;
    const double M = M_solar * solarMassKg;

    // Einstein angular radius
    const double thetaE_rad = sqrt(
        (4.0 * G * M * D_LS)
        / (c * c * D_L * D_S)
    );

    // Convert radians to arcseconds
    const double R_E = thetaE_rad * radToArcsec;

    int width, height, channels;
    unsigned char* data = stbi_load("Galacxy_Gravity_Lensing.png", &width, &height, &channels, 3);
    if (!data) {
        cerr << "読み込み失敗: " << stbi_failure_reason() << '\n';
        return 1;
    }
    cout << "size: " << width << " x " << height << ", 元のチャンネル数: " << channels << '\n';

    // 画像の一辺のピクセル数
    const int N = min(width, height);
    const double centerX = N / 2.0;
    const double centerY = N / 2.0;

    // 画像の角度範囲と物理的な長さ
    constexpr double fovInThetaE = 2.0;
    const double image_Ang_Size = 2.0 * fovInThetaE * R_E; // arcsec
    const double image_Length = 2.0 * tan(fovInThetaE * thetaE_rad) * D_S; //m

    // 1ピクセル当たりの物理的な長さ
    const double pixel_Size = image_Length / N;

    vector<unsigned char> out(N * N * 3, 0);

    auto invLensedPixel = [&](int x, int y, int& srcX, int& srcY) -> bool {
        const double dx = x - centerX;
        const double dy = y - centerY;
        const double distR = sqrt(dx * dx + dy * dy);

        if (distR == 0) {
            return false;
        }

        const double theta = atan(distR * pixel_Size / D_S);
        
        const double beta = theta - thetaE_rad * thetaE_rad / theta;

        const double scale = beta / theta;

        srcX = static_cast<int>(round(centerX + dx * scale));
        srcY = static_cast<int>(round(centerY + dy * scale));

        return true;
        };

    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            int srcX, srcY;
            if (!invLensedPixel(x, y, srcX, srcY))continue;
            if (srcX < 0 || srcX > N - 1 || srcY< 0 || srcY > N - 1)continue;
            for (int ch = 0; ch < 3; ++ch) {
                out[(y * N + x) * 3 + ch] = data[(srcY * width + srcX) * 3 + ch];
            }
        }
    }

    cout << scientific << setprecision(4);
    cout << "Einstein angle (rad): "
        << thetaE_rad << '\n';
    cout << "Einstein radius (arcsec): "
        << R_E << '\n';

    cout << "image_Ang_Size =" << image_Ang_Size << " arcsec\n";
    cout << "image_Length =" << image_Length << " m\n";
    cout << "pixel_Size =" << pixel_Size << " m\n";

    stbi_write_png("lensed.png", N, N, 3, out.data(), N * 3);
    stbi_image_free(data);

    return 0;
}
