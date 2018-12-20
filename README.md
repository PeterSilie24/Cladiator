# Cladiator

## Description

**Cladiator** is an **easy-to-use** class diagram editor created with the **Qt library** that should work with **Linux**, **Windows**, and **Mac OS** (although I do not have a Mac to test it).

The project was initiated by my former computer science teacher at the Gymnasium (as the German equivalent of high school is called). He wanted a small and easy-to-use class diagram editor for his lessons, as he was not satisfied with the existing ones.

## Build
**Linux**

* Make sure you have **Qt** and the default building tools like **make**, **gcc** and **g++** installed.

```
# install dependencies on Debian based systems
apt-get install build-essential qt5-default

```

* Once you have all necessary **dependencies** installed, you can **build** and **run** the application as follows:

```
# clone repository from GitHub
git clone https://github.com/PeterSilie24/Cladiator.git

# build
cd Cladiator
qmake && make

# run
cd bin
./cladiator
```

**Windows**

* First you have to **download** and **install** **Qt** from https://www.qt.io/download.

* Then **download** the **repository** from GitHub (https://github.com/PeterSilie24/Cladiator.git).

* If you have downloaded it as a **Zip** file, **extract** it first and then **navigate** to the **Cladiator** directory.

* Open the **cladiator.pro** file with **Qt Creator** and configure the project.

* Finally **build** the project with Qt Creator.

**Alternatively**, you can **download** a **pre** **compiled** **release** from https://github.com/PeterSilie24/Cladiator/releases.

## Install
**Linux**

* After the build process, the application can be installed as follows:

```
# install (you probably need to have root access)
make install
```
