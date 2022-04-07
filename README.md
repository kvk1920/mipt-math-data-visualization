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
Первым аргументом путь до графа в формате GraphML, вторым - путь до результата в формате svg
```shell
./vis-tree data/tree/bamboo.xml output.svg
```
