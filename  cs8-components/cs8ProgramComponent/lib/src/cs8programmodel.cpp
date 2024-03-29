//
// C++ Implementation: cs8programmodel
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8programmodel.h"
#include "cs8program.h"
#include <QDebug>
#include <QFont>
cs8ProgramModel::cs8ProgramModel(QObject *parent) :
    QAbstractListModel(parent) {
    m_cellPath="";
}

cs8ProgramModel::~cs8ProgramModel() {
    foreach(cs8Program *program, m_programList)
        program->deleteLater();
}

/*!
 \fn cs8ProgramModel::rowCount(const QModelIndex & index)
 */
int cs8ProgramModel::rowCount(const QModelIndex & /*index*/) const {
    return m_programList.count();
}

/*!
 \fn cs8ProgramModel::colCount(const QModelIndex & index)
 */
int cs8ProgramModel::colCount(const QModelIndex & /*index*/) const {
    return 2;//m_programList.count();
}

cs8Program* cs8ProgramModel::getProgramByName(const QString & name) {
    for (int i = 0; i < m_programList.count(); i++) {
        if (m_programList.at(i)->name().compare(name) == 0)
            return m_programList.at(i);
    }
    return 0;
}

void cs8ProgramModel::setCellPath(const QString &path)
{
    m_cellPath=path;
}

void cs8ProgramModel::append(cs8Program *program)
{
    m_programList.append (program);
    reset ();
}

QList<cs8Program*> cs8ProgramModel::publicPrograms() {
    QList<cs8Program*> out;
    foreach(cs8Program* program,m_programList)
    {
        qDebug() << program->name ();
        if (program->isPublic())
            out << program;
    }
    return out;
}

QList<cs8Program*> cs8ProgramModel::privatePrograms() {
    QList<cs8Program*> out;
    foreach(cs8Program* program,m_programList)
    {
        if (!program->isPublic())
            out << program;
    }
    return out;
}

/*!
 \fn cs8ProgramModel::data(const QModelIndex & index, int role)
 */
QVariant cs8ProgramModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role == Qt::FontRole) {
        if (m_programList.at(index.row())->isPublic()) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }

    if (role == Qt::DisplayRole){
        switch (index.column()){
        case 0:
            return m_programList.at(index.row())->name();
            break;

        case 1:
            return m_programList.at(index.row())->definition();
            break;
        }
    }

    if (role == Qt::UserRole)
        return m_programList.at(index.row())->val3Code(false);

    ///TODO why compiler error here?
    /*
        if (role == Qt::UserRole + 1)
        return m_programList.at(index.row())->parameterModel();

    if (role == Qt::UserRole + 2)
        return m_programList.at(index.row())->localVariableModel();
                */
    if (role == Qt::UserRole + 10)
        return m_programList.at(index.row())->detailedDocumentation();

    if (role == Qt::UserRole + 11)
        return m_programList.at(index.row())->description();

    return QVariant();
}

cs8Program * cs8ProgramModel::createProgram(const QString & programName)
{
    cs8Program* program = new cs8Program(this);

    m_programList.append(program);
    connect(program, SIGNAL(globalVariableDocumentationFound(const QString & , const QString & )),
            this, SLOT(slotGlobalVariableDocumentationFound(const QString & , const QString & )));
    connect(program, SIGNAL(moduleDocumentationFound(const QString & )),
            this, SLOT(slotModuleDocumentationFound(const QString & )));
    connect(program, SIGNAL(mainPageDocumentationFound(const QString & )),
            this, SLOT(slotMainPageDocumentationFound(const QString & )));
    connect(program, SIGNAL(exportDirectiveFound(QString,QString)),
            this,SLOT(slotExportDirectiveFound(QString,QString)));
    connect(program, SIGNAL(unknownTagFound(QString,QString,QString)),
            this,SLOT(slotUnknownTagFound(QString,QString,QString)));
    connect (program,SIGNAL(modified()),this,SLOT(slotModified()));

    program->setCellPath(m_cellPath);
    if (!programName.isEmpty ())
        program->setName (programName);

    return program;
}

void cs8ProgramModel::addProgram(const QString & filePath) {
    //qDebug() << "cs8ProgramModel::addProgram () " << filePath;
    cs8Program* program = createProgram("");
    if (!program->open(filePath))
        qDebug() << "Opening program " << filePath << " failed";
    reset();
}

void cs8ProgramModel::slotGlobalVariableDocumentationFound(
        const QString & name, const QString & document) {
    emit globalVariableDocumentationFound(name, document);
}

void cs8ProgramModel::slotModuleDocumentationFound(const QString & document) {
    emit moduleDocumentationFound(document);
}

void cs8ProgramModel::slotMainPageDocumentationFound(const QString & document) {
    emit mainPageDocumentationFound(document);
}


void cs8ProgramModel::slotExportDirectiveFound(const QString &module, const QString &function)
{
    emit exportDirectiveFound (module, function);
}

void cs8ProgramModel::slotUnknownTagFound(const QString &tagType, const QString &tagName, const QString &tagText)
{
    emit unknownTagFound (tagType, tagName, tagText);
}

void cs8ProgramModel::slotModified()
{
    emit modified (true);
}

void cs8ProgramModel::clear() {
    m_programList.clear();
    reset();
}

cs8VariableModel* cs8ProgramModel::localVariableModel(const QModelIndex & index) {
    if (index.isValid())
        return m_programList.at(index.row())->localVariableModel();
    else
        return 0;
}

cs8VariableModel* cs8ProgramModel::parameterModel(const QModelIndex & index) {
    if (index.isValid())
        return m_programList.at(index.row())->parameterModel();
    else
        return 0;
}

cs8VariableModel *cs8ProgramModel::referencedGlobalVriableModel(const QModelIndex &index)
{
    if (index.isValid())
        return m_programList.at(index.row())->referencedGlobalVariables ();
    else
        return 0;
}

bool cs8ProgramModel::setData(const QModelIndex & index,
                              const QVariant & value, int role) {
    if (!index.isValid())
        return false;

    if (role == Qt::UserRole + 10)
        m_programList.at(index.row())->setDetailedDocumentation(value.toString());

    if (role == Qt::UserRole + 11)
        m_programList.at(index.row())->setDescription(value.toString());

    return true;
}

