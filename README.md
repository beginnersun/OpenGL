# OpenGL
局部(-1,1)-世界-观察-裁剪-屏幕空间
裁剪到屏幕空间的计算就是将比例与原点重新计算的过程（原点从中心调整为左下角）
屏幕空间：坐标原点在左下方

GSLS内建变量
渲染图元：一般为点、线(还有 线_lines_adjacency)、三角形(还有 三角形 triangles_adjacency)。
gl_Position (x,y,z,w) xyz代表三维空间坐标 w在裁剪空间计算时使用（用于确定是否在裁剪区域内）
gl_PointSize 当渲染图元为点时，可以设置gl_PointSize的大小，从而改变渲染结束后，屏幕上每个点的大小。
gl_FragCoord (xy,z,w) xy代表坐标 z代表深度，w暂未
