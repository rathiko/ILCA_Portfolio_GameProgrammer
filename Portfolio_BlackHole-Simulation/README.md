# BlackHole-Simulation

シュワルツシルト時空での光の軌道をRK4で数値計算し、
ブラックホールの影と降着円盤の見え方を再現したC++のシミュレーションです。

<img width="520" height="520" alt="frame_028" src="https://github.com/user-attachments/assets/1b7e90dd-cb00-4ea5-92df-725df47f9e2f" />

## 特徴
- 弱い重力場の近似(レンズ方程式)と、強い重力場の計算(レイトレーシング)の2段階
- 温度に応じた円盤の色と明るさ、ケプラー回転のアニメーション
- 光線の計算を1回だけ行い、各フレームは色付けのみにして高速化

## 開発環境
C++(標準ライブラリ)、Visual Studio 2022、stb_image

## 詳しい説明
`解説資料_ブラックホール_重力レンズシミュレーション.pdf` を参照してください。

## 参考
- https://github.com/ShaunFell/Gravitational-Lensing-Visualization
- https://github.com/YIHnb/schwarzschild-ray-tracer/blob/main/ref/grbh.py
