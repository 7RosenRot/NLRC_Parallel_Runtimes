### Структура файлов

1. _Part 1_: Реализация расположена в директории `Part_1/workspace/include/counter.hpp`

2. _Part 2_: Реализация расположена в директории `Part_2/workspace/include/counter.hpp`.

3. Проект использует google-тесты, которые подключаются при сборке проекта.

### Установка, сборка и запсук

```Bash
git clone https://github.com/7RosenRot/NLRC_Parallel_Runtimes.git
cd NLRC_Parallel_Runtimes/Part_<1/2>/

cmake -S . -B build
cmake --build build --parallel --config=Release

build/tests/Release/Part_<1/2>Tests.exe
```

*! Обратите внимание, что проекты находятся в разных директориях, для перехода и запуска используйте выбор <1/2> !*

### Немного от себя
Очень интересное задание, мне понравилось. Буду рад обратной связи!
- Telegram: [Shemetov_Daniil](https://t.me/Shemetov_Daniil)
- mail: lego.den.2005@mail.ru
