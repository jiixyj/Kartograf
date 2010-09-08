#ifndef GUARD_H
#define GUARD_H

#include <QtCore>

int flip;
int Rotate;
int daynight;
int cave;
int exclude;
int slide;
void f();

QTextStream cin(stdin, QIODevice::ReadOnly);
QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

#endif
