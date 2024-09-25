/****************************************************************************
** Meta object code from reading C++ file 'RenderController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../RenderController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RenderController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RenderController_t {
    QByteArrayData data[23];
    char stringdata0[374];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RenderController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RenderController_t qt_meta_stringdata_RenderController = {
    {
QT_MOC_LITERAL(0, 0, 16), // "RenderController"
QT_MOC_LITERAL(1, 17, 21), // "objectRotationChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 14), // "raytraceCalled"
QT_MOC_LITERAL(4, 55, 17), // "xTranslateChanged"
QT_MOC_LITERAL(5, 73, 5), // "value"
QT_MOC_LITERAL(6, 79, 17), // "yTranslateChanged"
QT_MOC_LITERAL(7, 97, 17), // "zTranslateChanged"
QT_MOC_LITERAL(8, 115, 28), // "fresnelRenderingCheckChanged"
QT_MOC_LITERAL(9, 144, 5), // "state"
QT_MOC_LITERAL(10, 150, 24), // "phongShadingCheckChanged"
QT_MOC_LITERAL(11, 175, 25), // "interpolationCheckChanged"
QT_MOC_LITERAL(12, 201, 21), // "shadowBoxCheckChanged"
QT_MOC_LITERAL(13, 223, 20), // "reflectionBoxChanged"
QT_MOC_LITERAL(14, 244, 20), // "refractionBoxChanged"
QT_MOC_LITERAL(15, 265, 20), // "monteCarloBoxChanged"
QT_MOC_LITERAL(16, 286, 22), // "orthographicBoxChanged"
QT_MOC_LITERAL(17, 309, 15), // "BeginScaledDrag"
QT_MOC_LITERAL(18, 325, 11), // "whichButton"
QT_MOC_LITERAL(19, 337, 1), // "x"
QT_MOC_LITERAL(20, 339, 1), // "y"
QT_MOC_LITERAL(21, 341, 18), // "ContinueScaledDrag"
QT_MOC_LITERAL(22, 360, 13) // "EndScaledDrag"

    },
    "RenderController\0objectRotationChanged\0"
    "\0raytraceCalled\0xTranslateChanged\0"
    "value\0yTranslateChanged\0zTranslateChanged\0"
    "fresnelRenderingCheckChanged\0state\0"
    "phongShadingCheckChanged\0"
    "interpolationCheckChanged\0"
    "shadowBoxCheckChanged\0reflectionBoxChanged\0"
    "refractionBoxChanged\0monteCarloBoxChanged\0"
    "orthographicBoxChanged\0BeginScaledDrag\0"
    "whichButton\0x\0y\0ContinueScaledDrag\0"
    "EndScaledDrag"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RenderController[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x0a /* Public */,
       3,    0,   95,    2, 0x0a /* Public */,
       4,    1,   96,    2, 0x0a /* Public */,
       6,    1,   99,    2, 0x0a /* Public */,
       7,    1,  102,    2, 0x0a /* Public */,
       8,    1,  105,    2, 0x0a /* Public */,
      10,    1,  108,    2, 0x0a /* Public */,
      11,    1,  111,    2, 0x0a /* Public */,
      12,    1,  114,    2, 0x0a /* Public */,
      13,    1,  117,    2, 0x0a /* Public */,
      14,    1,  120,    2, 0x0a /* Public */,
      15,    1,  123,    2, 0x0a /* Public */,
      16,    1,  126,    2, 0x0a /* Public */,
      17,    3,  129,    2, 0x0a /* Public */,
      21,    2,  136,    2, 0x0a /* Public */,
      22,    2,  141,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Float, QMetaType::Float,   18,   19,   20,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   19,   20,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   19,   20,

       0        // eod
};

void RenderController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RenderController *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->objectRotationChanged(); break;
        case 1: _t->raytraceCalled(); break;
        case 2: _t->xTranslateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->yTranslateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->zTranslateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->fresnelRenderingCheckChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->phongShadingCheckChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->interpolationCheckChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->shadowBoxCheckChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->reflectionBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->refractionBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->monteCarloBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->orthographicBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->BeginScaledDrag((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 14: _t->ContinueScaledDrag((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 15: _t->EndScaledDrag((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RenderController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_RenderController.data,
    qt_meta_data_RenderController,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RenderController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RenderController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RenderController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RenderController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
