# GalacticMoon

惑星やブラックホールの重力に引っ張られながら戦う、3Dシューティングゲームです。
重力による軌道の変化を、プレイヤーが予測して対応できる手触りに調整しています。

<img width="1919" height="1079" alt="GamePlay_GalacticMoon" src="https://github.com/user-attachments/assets/8d9af88f-377b-40f1-a93a-9b0c0a6371f2" />

- 開発環境:Unreal Engine 5.5.4 / C++ / Blueprint
- 開発期間:約1年(学業と並行) / 1人で開発

## 見てほしいコード

| ファイル | 内容 |
| --- | --- |
| [BHOrbitPlayerComponent](Source_Game①_GalacticMoon/SpaceShooting/BHOrbitPlayerComponent.cpp) | ブラックホール周辺の軌道を、固定タイムステップとRK4で計算 |
| [BlackHoleComponent](Source_Game①_GalacticMoon/SpaceShooting/BlackHoleComponent.cpp) | 相対論的な補正を含む加速度の計算 |
| [GravityPawnMovement](Source_Game①_GalacticMoon/SpaceShooting/GravityPawnMovement.cpp) | 標準の移動処理を拡張し、移動の計算前に惑星の重力を加える |
| [AsteroidSpawner](Source_Game①_GalacticMoon/SpaceShooting/AsteroidSpawner.cpp) | Instanced Static Meshによる大量の障害物の配置と管理 |

### BHOrbitPlayerComponent
- **固定タイムステップ**:経過時間を蓄積し、一定の間隔で計算を進めることで、フレームレートが変わっても同じ軌道になるようにしています
- **積分と力の計算の分離**:RK4の処理は加速度を関数として受け取るだけにし、重力のモデルを差し替えても同じ積分処理を使えるようにしています
- 研究で使っていたPythonの軌道計算を、1ステップずつ状態を更新するゲーム向けの構造に作り直して移植しました

### GravityPawnMovement
- FloatingPawnMovementでは、重力を後から加えると移動の計算に打ち消されるため、親の処理より前に速度へ重力を加える形で拡張しました

## そのほかの実装
- 敵の挙動を、出現・移動・攻撃・状態遷移の4つのコンポーネントに分離
- 非同期でのレベル読み込み(AsyncLevelLoader)
- ロックオン、ホーミング、オーバーヒート、チュートリアルなどはBlueprintで実装

## 詳しい説明
[提出作品説明①_GalacticMoon.pdf] を参照してください。
