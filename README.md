# mipt-math-data-visualization

## Сборка
```shell
mkdir build
cd build
cmake ..
cmake --build .
```

Результат будет находиться в корне проекта (не в `build`)

## Запуск
### vis-tree
Первым аргументом путь до графа в формате GraphML, вторым - путь до результата в формате svg
```shell
./vis-tree data/tree/bamboo.xml output.svg
```

### vis-dag
Первым аргументом передаётся путь до графа, вторым - путь до картинки, третий - опциональный параметр W
```shell
./vis-graph data/dag/dag_9_13.xml coffman_3.svg 3
./vis-graph data/dag/dag_9_13.xml coffman_2.svg 2
./vis-graph data/dag/dag_9_13.xml lp.svg
```

### vis-labels
Первым аргументом путь до файла, вторым - путь до результата.
```shell
./vis-labels data/labels/sample1.txt sample1.svg
./vis-labels data/labels/chain.txt chain.svg
./vis-labels data/labels/impossible.txt impossible.svg
```
