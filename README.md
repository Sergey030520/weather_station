<h1 align="center">Hi 👋, I'm Sergey Makarov</h1>
<p align="left"> <img src="https://komarev.com/ghpvc/?username=sergey030520&label=Profile%20views&color=0e75b6&style=flat" alt="sergey030520" /> </p>

- 🔭 I’m currently working on [weather_station](https://github.com/Sergey030520/weather_station)

- 👨‍💻 All of my projects are available at [https://github.com/Sergey030520](https://github.com/Sergey030520)


<h3 align="left">Languages and Tools:</h3>
<p align="left"> 
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"><img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/c/c-original.svg" alt="c" width="40" height="40"/> </a>
<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"><img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> </a> 
<a href="https://git-scm.com/" target="_blank" rel="noreferrer"> <img src="https://www.vectorlogo.zone/logos/git-scm/git-scm-icon.svg" alt="git" width="40" height="40"/> </a>
<a href="https://cmake.org/" rel="noreferrer"><img src="https://www.vectorlogo.zone/logos/cmake/cmake-icon.svg" alt="Cmake" width="40" height="40"/></a>
</p>

# weather_station

## Оглавление
1. [<p style='font-size: 20px'>Описание проекта</p>](#description_project)
2. [<p style='font-size: 20px'>Сборка проекта и тестирование</p>](#build_and_testing_project)
3. [<p style='font-size: 20px'>Инструкция по сборке проекта</p>](#manual_build_project)
## Описание программы<a name="description_project"></a>

Разработка информационно–управляющей системы метеостанции:
1. Приложение выполняет следующие задачи:
    + Опрос датчика температуры и часов реального времени (RTC), ведение журнала температурных показаний;
    + Пользовательский интерфейс по UART:
        + Задание текущей даты и времени системы;
        + Отображение текущей даты и времени системы;
        + Отображение данных о текущей температуре;
        + Получение журнала температурных показаний;
        + Очистка журнала температурных показаний. <br>

![software weather station](https://github.com/Sergey030520/weather_station/blob/master/ImagesProject/software_weather_station.png) <br>


## Сборка проекта и тестирование <a name="build_and_testing_project"></a>
Данный проект был собран и протестирован с помощью среды разработки [STM32 CUBE IDE](https://www.st.com/en/development-tools/stm32cubeide.html).
Программа разработана на языке C и протестирована на плате STM32F302R8TxLQFP64.

## Инструкция по сборке проекта <a name="manual_build_project"></a>

Сборка проекта производится с помощью среды разработки [STM32 CUBE IDE](https://www.st.com/en/development-tools/stm32cubeide.html).

