# Вещи, изученные при работе с камерой.
· **inline** - встроенная функция. Используется, чтобы ускорить программу ценой занимаемого
места. После того как вы определите встроенную функцию с помощью ключевого слова inline,
всякий раз когда вы будете вызывать эту функцию, компилятор будет заменять вызов функции
фактическим кодом из функции. Это позволяет значительно ускорить вызов функций с небольшим
кодом. Процесс вызова обычных функций - медленнее, чем процесс вызова встроенных
функций.

· **p.SetPerspectiveProj(30.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 1000.0f);**
первое - поле зрения по вертикали - угол обзора камеры. Второе и третье - ширина и высота
прямоугольной области, на которую будет осуществляться проекция. Четвёртое - позиция ближней Z
области. Пятое - позиция дальней Z плоскости.

**'UVN камера'** - одна из множества систем, характеризующих камеру. Идея в том, что камера
определяется следующими векторами:
N - Вектор от камеры к ее цели. Так же известен как вектор 'look at' в некоторой литературе о 3D.
Этот вектор соответствует Z оси.
V - Если стоять прямо, то этот вектор будет исходить из головы в небо. Если вы пишите симулятор
полетов, и один из них перевернут, то вектор будет указывать на землю. Этот вектор соответствует
оси Y.
U - Этот вектор выходит из камеры направо. Соответствует оси X.

· **WVP** расшифровывается как вид мировой проекции (World-View-Projection).

· Метод cross из урока есть в библиотеке glm. (glm::cross(vec3, vec3)). Операция возвращает
вектор, перпендикулярный плоскости, определяемой исходными векторами.

· Нормалиазция вектора тоже есть в glm-библиотеке.
____
При настройке с камерой в уроке 13 столкнулся с проблемой: вместо треугольника происходила отрисовка трапеции. Так и не смог решить проблему, поэтому отправляюсь
читать учебник **"Computer Graphics Programming in OpenGL Using C++ by V Scott Gordon John L Clevenger"**, выложенный в slack, в надежде найти решение и улучшить навыки
оперирования OpenGL и GLSL.
