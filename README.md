## MyEngine 作品

幻想的な世界観のレンダリングを目指し制作中

DirectX12を利用した自作エンジンです。

制作期間 約2年 制作人数 1人

## 機能

* Particleの実装
* MeshShderによる波の演出
* スキニングアニメーション
* Tiled Based Rendering による Forwrd+ の実装
  
## 注目してほしいポイント

TiledBasedRenderingによるForward+の実装によって大量のポイントライトの処理ができるようになっており

大量のライトによる視覚効果にこだわっています。

MeshShaderによるテッセレーションでパーティクルが床に落ちる際に波を発生させています。


  ![image](https://github.com/user-attachments/assets/59f0ae74-204f-4c64-b958-475d2b576c16)

## Forward+実装

ゲーム内で扱う視錐台をタイル状に分割します。

タイル状に分割した視錐台をカメラのアフィン行列を適用します。

アフィン変換を行ったタイル状の視錐台とライトの当たり判定を行い

タイルごとのバッファーにポイントライトのインデックスを登録していきます。

ピクセルシェーダーでライティングを行うとき、所属するタイルのライトのみとライティングを行います。

![image](https://github.com/user-attachments/assets/c606f037-c889-415b-bbaa-341d80589629)

このような実装でライティングを減らしています。

## 波の実装

波の起こる可能性のある範囲の球とMeshletの球で当たり判定をとり

波が起こるMeshletのみテッセレーションを行い波を起こしています。

![wkwkw](https://github.com/user-attachments/assets/100acb2e-1c32-46fb-b0cc-5a68fe19d439)

  
  ## 使用ライブラリ
* DirectXTex
* DirectXMesh
* asiimp
* ImGUI




  
