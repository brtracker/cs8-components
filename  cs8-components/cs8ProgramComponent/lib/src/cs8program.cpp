#include "cs8application.h"
#include "cs8program.h"
#include "cs8variable.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>

#define MAX_LENGTH 40
//
cs8Program::cs8Program( QObject *parent ) :
    QObject( parent )
{
    m_localVariableModel = new cs8LocalVariableModel( this );
    m_parameterModel = new cs8ParameterModel( this );
    m_referencedGlobalVarModel= new cs8VariableModel(this, cs8VariableModel::ReferencedGlobal);
    m_globalDocContainer=false;
    createXMLSkeleton();
}
//
/*!
 \fn cs8Program::cs8Program(const QString & filePath)
 */
/*
 cs8Program::cs8Program(const QString & filePath) :
 QObject() {
 m_localVariableModel = new cs8VariableModel(this);
 m_parameterModel = new cs8ParameterModel(this);
 open(filePath);
 }
 */

bool cs8Program::open( const QString &filePath )
{
    qDebug() << "cs8Program::open () " << filePath;

    QFile file( filePath );
    if ( !file.open( QIODevice::ReadOnly ) )
        return false;
    if ( !m_XMLDocument.setContent( &file ) )
    {
        file.close();
        return false;
    }
    file.close();

    m_filePath = filePath;
    return parseProgramDoc( m_XMLDocument );
}


void cs8Program::printChildNodes( const QDomElement &element )
{
    qDebug() << "Child nodes of: " << element.tagName();
    for ( int i = 0; i < element.childNodes().count(); i++ )
        qDebug() << i << element.childNodes().at( i ).nodeName();

}

void cs8Program::createXMLSkeleton()
{
    m_XMLDocument = QDomDocument();
    QDomProcessingInstruction process = m_XMLDocument.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"utf-8\"" );
    m_XMLDocument.appendChild( process );

    m_programsSection = m_XMLDocument.createElement( "Programs" );
    m_programsSection.setAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
    m_programsSection.setAttribute( "xmlns", "http://www.staubli.com/robotics/VAL3/Program/2" );
    m_XMLDocument.appendChild( m_programsSection );

    m_programSection = m_XMLDocument.createElement( "Program" );
    m_programsSection.appendChild( m_programSection );

    m_descriptionSection = m_XMLDocument.createElement( "Description" );
    m_programSection.appendChild( m_descriptionSection );

    m_paramSection = m_XMLDocument.createElement( "Parameters" );
    m_paramSection.setAttribute( "xmlns", "http://www.staubli.com/robotics/VAL3/Param/1" );
    m_programSection.appendChild( m_paramSection );

    m_localSection = m_XMLDocument.createElement( "Locals" );
    m_programSection.appendChild( m_localSection );

    m_codeSection = m_XMLDocument.createElement( "Code" );
    m_programSection.appendChild( m_codeSection );
    setCode ("begin\nend");
}

/*!
 \fn cs8Program::parseProgramDoc(const QDomDocument & doc)
 */
bool cs8Program::parseProgramDoc( const QDomDocument &doc )
{
    m_programsSection = doc.documentElement();


    //printChildNodes(m_programsSection);
    m_programSection = m_programsSection.firstChild().toElement();


    //printChildNodes(m_programSection);
    if ( m_programSection.isNull() )
        qDebug() << "Reading program section failed";



    m_descriptionSection = m_programsSection.elementsByTagName( "Description" ).at( 0 ).toElement();
    if ( m_descriptionSection.isNull() )
    {
        m_descriptionSection = m_XMLDocument.createElement( "Description" );
        m_programSection.insertBefore( m_descriptionSection, m_programSection.firstChild() );
    }
    m_paramSection = m_programSection.elementsByTagName( "Parameters" ).at( 0 ).toElement();
    m_localSection = m_programSection.elementsByTagName( "Locals" ).at( 0 ).toElement();

    //qDebug() << m_programSection.elementsByTagName("Code").at(0).nodeType();
    m_codeSection =  m_programSection.elementsByTagName( "Code" ).at( 0 ).toElement();
    if ( m_codeSection.isNull() )
        qDebug() << "Reading code section failed";


    //qDebug() << "program name: " << name () << m_programSection.attribute ("name");
    QDomNodeList childList;

    childList = m_localSection.elementsByTagName( "Local" );
    for ( int i = 0; i < childList.count(); i++ )
    {
        QDomElement item = childList.at( i ).toElement();
        m_localVariableModel->addVariable( item );
    }

    childList = m_paramSection.elementsByTagName( "Parameter" );
    for ( int i = 0; i < childList.count(); i++ )
    {
        QDomElement item = childList.at( i ).toElement();
        m_parameterModel->addVariable( item );
    }

    //
    QStringList referencedVariables=variableTokens (true);
    cs8Application* app=qobject_cast<cs8Application*>(parent ());
    if (app)
    {
        foreach(QString var,referencedVariables)
        {
            if (app->globalVariableModel ()->variableNameList ().contains (var) &&
                    !m_localVariableModel->variableNameList ().contains (var) &&
                    !m_referencedGlobalVarModel->variableNameList ().contains (var))
            {
                cs8Variable *refVar=new cs8Variable();
                cs8Variable *globalVar=app->globalVariableModel ()->getVarByName (var);

                refVar->setGlobal (true);
                refVar->setName (globalVar->name ());
                refVar->setType (globalVar->type ());
                refVar->setAllSizes (globalVar->allSizes ());
                m_referencedGlobalVarModel->addVariable (refVar);
            }
        }
    }
    parseDocumentation( val3Code() );
    // update token list
    m_variableTokens=variableTokens (false);
    return true;
}

void cs8Program::tidyUpCode( QString &code )
{
    // remove empty lines after "end"
    QStringList list = code.split( "\n" );
    while ( list.last().simplified().isEmpty() )
        list.removeLast();

    code = list.join( "\n" );
}
QDomElement cs8Program::programsSection() const
{
    return m_programsSection;
}

void cs8Program::setProgramsSection(const QDomElement &programsSection)
{
    m_programsSection = programsSection;
}



void cs8Program::setApplicationDocumentation(const QString &applicationDocumentation)
{
    m_applicationDocumentation = applicationDocumentation;
}

void cs8Program::setMainPageDocumentation(const QString &applicationDocumentation)
{
    m_mainPageDocumentation = applicationDocumentation;
}



// returns the code without documentation header
QString cs8Program::val3Code( bool withDocumentation )
{
    // qDebug() << m_codeSection.text();
    return withDocumentation ? m_codeSection.text() : extractCode( m_codeSection.text() );
}

QString cs8Program::toCSyntax()
{
    QString val3_ = val3Code(false);
    val3_ = val3_.replace( QString( "call " ), QString( "" ), Qt::CaseSensitive );
    val3_ = val3_.replace( ":", "." );
    return val3_;
}

QString cs8Program::extractCode( const QString &code_ ) const
{
    QString code = code_;
    // extract code
    code.replace( "\r\n", "\n" );
    QStringList list = code.split( "\n" );
    int start = list.indexOf( QRegExp( "\\s*begin\\s*" ) );
    int stop = start + 1;
    while ( list.at( stop ).contains( QRegExp( "^\\s*//.*" ) ) || list.at( stop ).simplified().isEmpty() )
        stop++;
    for ( int i = stop - 1; i > start; i-- )
        list.removeAt( i );

    return list.join( "\n" );
}

// return a list of variable tokens
QStringList cs8Program::variableTokens(bool onlyModifiedVars)
{
    QStringList list;
    QString code = val3Code( true );
    foreach( QString line, code.split( "\n" ) )
    {
        QString l = line.trimmed();
        // ignore comment lines
        if ( !l.startsWith( "//" ) )
        {
            // remove string from code line
            QRegExp rx;
            rx.setPattern( "\\\".*\\\"" );
            rx.setMinimal( true );
            int pos = 0;
            while ( rx.indexIn( l, pos ) != -1 )
                l.replace( rx, " " );
            //
            if (onlyModifiedVars)
                rx.setPattern( "(\\w+)\\s*=" );
            else
                rx.setPattern( "(\\w+)" );
            rx.setMinimal( false );
            pos = 0;
            while ( ( pos = rx.indexIn( l, pos ) ) != -1 )
            {
                list << rx.cap( 1 );
                pos += rx.matchedLength();
            }
        }
    }
    return list;
}

void cs8Program::setCode( const QString &code, bool parseDoc_ )
{
    QDomCDATASection data = m_XMLDocument.createCDATASection( code );
    m_programSection.removeChild( m_codeSection );
    m_codeSection = m_XMLDocument.createElement( "Code" );

    //while (m_codeSection.hasChildNodes ())
    //    m_codeSection.removeChild (m_codeSection.firstChildElement ());
    m_codeSection.appendChild( data );
    m_programSection.appendChild( m_codeSection );
    if (parseDoc_)
        parseProgramDoc(m_XMLDocument);
}

void cs8Program::copyFromParameterModel( cs8ParameterModel *sourceModel )
{
    m_parameterModel->clear();
    foreach( cs8Variable * param, sourceModel->variableList() )
    {
        QDomElement element = param->element().cloneNode( true ).toElement();
        cs8Variable *p = new cs8Variable( element, param->description() );
        //qDebug() << p->name () << ":" << p->definition ();

        /*
            p->setDescription (param->description ());
            p->setName (param->name ());
            p->setGlobal (param->isGlobal ());
            p->setType (param->type ());
            p->setUse (param->use ());
            */
        m_parameterModel->addVariable( element, param->description() );
    }
}

void cs8Program::addTag(const QString &tagType, const QString &tagName, const QString &tagText)
{
    m_tags.insertMulti( tagType, tagName + " " + tagText );
}

void cs8Program::clearDocumentationTags()
{
    m_tags.clear ();
}

void cs8Program::setWithUndocumentedSymbols(bool withUndocumentedSymbols)
{
    m_parameterModel->setWithUndocumentedSymbols (withUndocumentedSymbols);
    m_localVariableModel->setWithUndocumentedSymbols (withUndocumentedSymbols);
}

QStringList cs8Program::referencedVariables() const
{
    return m_variableTokens;
}

QMap<int, QString> cs8Program::todos()
{
    QString code=val3Code (true);
    QMap<int, QString> todos;
    qDebug() << "Check todos in " << name ();
    QRegExp rx;
    rx.setPattern ("^\\s*//[/\\\\]{1}\\s*TODO.*");
    int codeLine=0;
    foreach(QString line,code.split ("\n"))
    {
        if (rx.indexIn (line)!=-1)
            todos.insert (codeLine,line);
        codeLine++;
    }
    return todos;
}

QStringList cs8Program::getCalls()
{
    QString code=val3Code (true);
    qDebug() << "Check todos in " << name ();
    QRegExp rxCall, rxTaskCreate;
    rxCall.setPattern ("^\\s*call (\\w*)\\(");
    rxTaskCreate.setPattern("^(\\s*taskCreate\\s*\\\"*\\w*\\\"*\\s*,\\s*\\w*\\s*,\\s*)(\\w*)\\(");
    QStringList list;
    foreach(QString line,code.split ("\n"))
    {
        if (rxCall.indexIn (line)!=-1)
        {
            if (rxCall.captureCount()==1)
                list << rxCall.cap(1);
        }
        else if (rxTaskCreate.indexIn(line)!=-1)
        {
            if (rxTaskCreate.captureCount()==2)
                list << rxTaskCreate.cap(2);
        }
    }
    return list;
}

QString cs8Program::extractDocumentation( const QString &code_ )
{
    //
    //extract documentation section
    QString documentationSection = code_;
    documentationSection.replace( "\r\n", "\n" );
    // revert automatic correction done by Val3 Studio
    documentationSection.replace( "//\\endIf", "//\\endif" );
    documentationSection.remove( 0, documentationSection.indexOf( "begin\n" ) + 8 );
    QStringList documentationList = documentationSection.split( "\n" );
    //
    // remove code section
    // search first non comment line
    int i = 0;
    while ( !documentationList.at( i ).simplified().startsWith( ( "//_" ) ) && (documentationList.at( i ).simplified().startsWith( ( "//" ) ) || documentationList.at( i ).simplified().isEmpty() ))
        i++;
    // remove lines starting from found first non comment line
    while ( documentationList.count() > i )
        documentationList.removeLast();

    return documentationList.join( "\n" );
}
bool cs8Program::globalDocContainer() const
{
    return m_globalDocContainer;
}

void cs8Program::setGlobalDocContainer(bool globalDocContainer)
{
    m_globalDocContainer = globalDocContainer;
}


/*!
 \fn cs8Program::parseDocumentation( QString & code)
 */
/*
 void cs8Program::parseDocumentation(QString & code) {

 QString text = code;

 // remove header from code
 QString out;
 if (true) {
 code.replace("\r\n", "\n");
 QStringList list = code.split("\n");
 int start = list.indexOf(QRegExp("\\s*begin\\s*"));
 int stop = start + 1;
 while (list.at(stop).contains(QRegExp("^\\s*�//.*")))

 stop++;
 for (int i = stop - 1; i > start; i--)
 list.removeAt(i);
 code = list.join("\n");
 //out=code.replace(start,stop-start,QString());
 }
 text.replace("\r\n", "\n");
 text.remove(0, code.indexOf("begin\n") + 8);
 QStringList list = text.split("\n");

 int i = 0;
 for (i = 0; i < list.count(); i++) {
 if (!(list[i] = list[i].simplified()).startsWith("//"))
 break;
 }
 while (list.count() > i)
 list.removeLast();

 int state = 0;
 QString itemName = "";
 QString itemDescr = "";
 QStringList itemList;
 QString line;
 i = 0;
 while (i < list.count()) {
 line = list.at(i);
 QString item;
 if (line.startsWith("//!")) {
 item = line;
 i++;
 }

 while (i < list.count() && !((line = list.at(i)).startsWith("//!"))) {
 item += line.replace(QRegExp("^//"), "\n");
 i++;
 }
 itemList << item;
 }

 foreach ( QString line,itemList ) {
 if (line.startsWith("//!brief")) {
 line.remove("//!brief\n");
 m_description = line;
 }

 if (line.startsWith("//!param")) {
 line.remove("//!param");
 itemName = line.left(line.indexOf(":")).simplified();
 itemDescr
 = line.right(line.length() - line.indexOf(":") - 1).simplified();
 if (m_parameterModel->getVarByName(itemName) != 0)
 m_parameterModel->getVarByName(itemName)->setDescription(
 itemDescr);
 }
 if (line.startsWith("//!local")) {
 line.remove("//!local");
 itemName = line.left(line.indexOf(":")).simplified();
 itemDescr = line.simplified();
 if (m_localVariableModel->getVarByName(itemName) != 0)
 m_localVariableModel->getVarByName(itemName)->setDescription(
 itemDescr);
 }
 if (line.startsWith("//!global")) {
 line.remove("//!global");
 line = line.simplified();
 itemName = line.left(line.indexOf(" ")).simplified();
 itemDescr = line.simplified();
 qDebug() << "global var: " << itemName << ":" << itemDescr;
 emit globalVariableDocumentationFound(itemName, itemDescr);
 }
 }
 }
 */

void cs8Program::parseDocumentation( const QString &code_ )
{

    QString documentation = extractDocumentation( code_ );
    QString tagType;
    QString tagName;
    QString tagText;
    m_tags.clear();

    qDebug() << "parseDocumentation: " << name();
    foreach( QString line, documentation.split( "\n" ) )
    {
        line = line.simplified();
        // process a complete tag before starting the next tag
        if ( ( line.startsWith( "//!" ) || line.startsWith( "//\\" ) ) && !tagType.isEmpty() )
        {
            // remove trailing array indexes from tag name
            tagName = tagName.remove( QRegExp( "\\[\\d*\\]" ) );
            //qDebug() << "process tag:" << tagType << ":" << tagName << ":" << tagText;
            if ( tagType == "param" )
            {
                if ( m_parameterModel->getVarByName( tagName ) != 0 )
                    m_parameterModel->getVarByName( tagName )->setDescription(
                                tagText );
            }
            else if ( tagType == "local" || tagType == "var" )
            {
                if ( m_localVariableModel->getVarByName( tagName ) != 0 )
                    m_localVariableModel->getVarByName( tagName )->setDescription(
                                tagText );
            }
            else if ( tagType == "refGlobal" )
            {
                if ( m_referencedGlobalVarModel->getVarByName( tagName ) != 0 )
                    m_referencedGlobalVarModel->getVarByName( tagName )->setDescription(
                                tagText );
            }
            else if ( tagType == "global" )
            {
                //qDebug() << "global var: " << tagName << ":" << tagText;
                emit globalVariableDocumentationFound( tagName, tagText );
            }
            else if ( tagType == "brief" )
            {
                setDescription( tagText );
            }
            else if ( tagType == "doc" )
            {
                setDetailedDocumentation( tagText );
            }
            else if ( tagType == "module" )
            {
                qDebug() << "module doc: " << tagName << ":" << tagText;
                emit moduleDocumentationFound( tagText );
            }
            else if ( tagType == "mainpage" )
            {
                qDebug() << "main page doc: " << tagName << ":" << tagText;
                emit mainPageDocumentationFound( tagText );
            }
            else if ( tagType == "export" )
            {
                tagText = tagText.simplified();
                qDebug() << "export directive: " << tagName << ":" << tagText;
                if ( tagText.isEmpty() )
                    tagText = name();
                m_tags.insertMulti( tagType, tagName + " " + tagText );
                emit exportDirectiveFound( tagName, tagText );
            }
            else if ( tagType == "copyright" )
            {
                setCopyrightMessage( tagText );
            }
            else
            {
                if ( tagText.isEmpty() )
                    tagText = tagName;
                m_tags.insertMulti( tagType, tagText );
                emit unknownTagFound( tagType, tagName, tagText );
                //unknown tag type
                //m_description += QString("\n\\%1 %2\n%3").arg(tagType).arg(tagName).arg(tagText);
            }
        }
        // a new tag
        if ( line.startsWith( "//!" ) || line.startsWith( "//\\" ) )
        {

            tagText = "";
            tagName = "";
            // set tag type
            tagType = line.remove( "//!" ).remove( "//\\" ).simplified();
            tagType = tagType.left( tagType.indexOf( " " ) ).simplified();
            // set tag name
            QRegExp rx;
            rx.setPattern( "^\\s*" + tagType );
            tagText = line.remove( rx ).simplified();
            rx.setPattern( "^\\w*(\\s|$)" );
            int pos = rx.indexIn( tagText );
            if ( pos != -1 )
                tagName = rx.cap().simplified();
            else
                tagName = "";
            // set tag text
            tagText.remove( rx );
            rx.setPattern( "^:\\s*" );
            tagText.remove( rx );
            qDebug() << "new tag:" << tagType << ":" << tagName << ":"
                     << tagText;
        }
        else
        {
            // Read tag text from code lines and prepend a \n at end of each read line.
            // However, do not prepend a \n if it is the first line.
            tagText += ( tagText.isEmpty() ? "" : "\n" ) + line.remove( 0, 2 ); //"\n" + line.remove(0, 2);
            // remove double \n at end of tag text
            while (tagText.right (2)=="\n\n")
                tagText.chop (1);
        }

    }
    // process a complete tag if there is one available at the end
    if ( !tagType.isEmpty() )
    {
        qDebug() << "process remaining tag:" << tagType << ":" << tagName << ":" << tagText;
        if ( tagType == "param" )
        {
            if ( m_parameterModel->getVarByName( tagName ) != 0 )
                m_parameterModel->getVarByName( tagName )->setDescription( tagText );
        }
        else if ( tagType == "local" || tagType == "var" )
        {
            if ( m_localVariableModel->getVarByName( tagName ) != 0 )
                m_localVariableModel->getVarByName( tagName )->setDescription(
                            tagText );
        }
        else if ( tagType == "global" )
        {
            qDebug() << "global var: " << tagName << ":" << tagText;
            emit globalVariableDocumentationFound( tagName, tagText );
        }
        else if ( tagType == "refGlobal" )
        {
            if ( m_referencedGlobalVarModel->getVarByName( tagName ) != 0 )
                m_referencedGlobalVarModel->getVarByName( tagName )->setDescription(
                            tagText );
        }
        else if ( tagType == "brief" )
        {
            setDescription( tagText );
        }
        else if ( tagType == "doc" )
        {
            setDetailedDocumentation( tagText );
        }
        else if ( tagType == "module" )
        {
            qDebug() << "module doc: " << tagName << ":" << tagText;
            emit moduleDocumentationFound( tagText );
        }
        else if ( tagType == "mainpage" )
        {
            qDebug() << "mainpage doc: " << tagName << ":" << tagText;
            emit mainPageDocumentationFound( tagText );
        }
        else if ( tagType == "export" )
        {
            tagText = tagText.simplified();
            if ( tagText.isEmpty() )
                tagText = name();
            m_tags.insertMulti( tagType, tagName + " " + tagText );
            emit exportDirectiveFound( tagName, tagText );
        }
        else if ( tagType == "copyright" )
        {
            setCopyrightMessage( tagText );
        }
        else
        {
            if ( tagText.isEmpty() )
                tagText = tagName;
            m_tags.insertMulti( tagType, tagText );
            emit unknownTagFound( tagType, tagName, tagText );
            //m_description += QString("\n\\%1 %2\n%3").arg(tagType).arg(tagName).arg(tagText);
        }
    }
    //qDebug() << documentationList;

}

QString cs8Program::description() const
{
    return m_briefDescription;
}

void cs8Program::setCellPath( const QString &path )
{
    m_cellPath = path;
}

QString cs8Program::cellFilePath() const
{
    QString pth = QDir::toNativeSeparators( m_filePath );
    pth = pth.replace( m_cellPath + "/usr/usrapp", "Disk://" );
    pth = QDir::toNativeSeparators( pth );
    return pth;
}

QString cs8Program::documentation( bool withPrefix ) const
{
    qDebug() << "documentation: " << name();
    QString out;
    QString prefix = withPrefix ? "///" : "";
    /*
    QStringList list = m_detailedDocumentation.split( "\\n" );
    list.prepend( "\n" );
    list.prepend(description() );
    list.prepend( "//\\brief" );
    */
    QStringList list;
    list << "\\brief" << description().split("\n") << "\n" << m_detailedDocumentation.split( "\n" );
    //list.removeLast();
    bool inCodeSection = false;
    int indentation = 0;
    foreach( QString str, list )
    {
        if ( str.contains( "<code>" ) )
        {
            inCodeSection = true;
            out += prefix + "<br>\n";
        }
        if ( str.contains( "</code>" ) )
        {
            inCodeSection = false;
            out += prefix + "<br>\n";
        }
        if ( inCodeSection )
        {
            qDebug() << str << "indent: " << indentation;
            if ( str.simplified().indexOf( QRegExp( "begin |if |for | while |do |switch| case" ) ) == 0 )
            {
                str = QString().fill( QChar( ' ' ), indentation * 4 ) + str.simplified();
                indentation++;
            }
            if ( str.simplified().indexOf( QRegExp( "end|until |break" ) ) == 0 )
            {
                indentation--;
                indentation = qMax( 0, indentation );
                str = QString().fill( QChar( ' ' ), indentation * 4 ) + str.simplified();
            }
        }
        out += prefix + str + ( inCodeSection ? "<br>" : "" ) + "\n";
    }
    //out += "\n";
    //out =list.join("\n");
    foreach( cs8Variable * parameter, m_parameterModel->variableList() )
    {
        out += parameter->documentation(true,false);
    }
    return out;
}

void cs8Program::setDescriptionSection()
{
    QString txt;
    txt = m_briefDescription;
    while (txt.endsWith ("\n"))
        txt.chop (1);
    // retrieve documentation of parameters
    for ( int i = 0; i < m_parameterModel->variableList().count() - 1; i++ )
    {
        if ( i == 0 )
            txt += "\n";
        if (m_parameterModel->variableList ().at (i)->documentation (false,false).length ()>2)
            txt += m_parameterModel->variableList().at( i )->name ()+": "+m_parameterModel->variableList().at( i )->description();
    }

    QDomNode node;
    while ( m_descriptionSection.childNodes().count() > 0 )
    {
        node = m_descriptionSection.childNodes().at( 0 );
        m_descriptionSection.removeChild( node );
    }
    QDomCDATASection data = m_XMLDocument.createCDATASection( txt );
    m_descriptionSection.appendChild( data );
}

void cs8Program::setDescription( const QString &theValue )
{
    m_briefDescription = theValue;
}

void cs8Program::setDetailedDocumentation( const QString &doc )
{
    m_detailedDocumentation = doc;
}

QString cs8Program::detailedDocumentation() const
{
    return m_detailedDocumentation;
}

/*
bool cs8Program::save(const QString & projectPath, bool withCode) {
    qDebug() << "Saving progam " << m_name;
    QDomDocument doc;
    QDomElement programList = doc.createElement("programList");
    programList.setAttribute("xmlns", "ProgramNameSpace");
    doc.appendChild(programList);

    QDomElement program = doc.createElement("program");
    program.setAttribute("name", name());
    program.setAttribute("public", isPublic() ? "true" : "false");
    programList.appendChild(program);


    QDomElement descriptionElement=doc.createElement("description");
    //descriptionElement.setAttribute("xmlns","ProgramNameSpace");
    program.appendChild(descriptionElement);
    program.appendChild(m_parameterModel->document(doc));
    program.appendChild(m_localVariableModel->document(doc));

    QDomElement source = doc.createElement("source");
    program.appendChild(source);

    QDomElement code = doc.createElement("code");
    source.appendChild(code);

    QDomText codeText;
    if (withCode)
    {
        codeText = doc.createTextNode(toDocumentedCode());
    }
    else
    {
        if (name()!="stop")
        {
            codeText = doc.createTextNode("begin\n"
                                          "logMsg(libPath())\n"
                                          "put(sqrt(-1))\n"
                                          "end");
        }
        else
        {
            codeText = doc.createTextNode("begin\n"
                                          "end");
        }
    }
    code.appendChild(codeText);

    QFile file(projectPath + "/" + fileName());
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream stream(&file);
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    stream << doc.toString();
    file.close();

    return true;
}
*/


bool cs8Program::save( const QString &projectPath, bool withCode )
{
    Q_ASSERT(!name ().isEmpty ());
    qDebug() << "Saving progam " << name();
    setDescriptionSection();
    QString codeText;
    if ( withCode )
    {
        codeText = toDocumentedCode();
    }
    else
    {
        if ( name() != "stop" )
        {
            codeText = ( "begin\n"
                         "logMsg(libPath())\n"
                         "put(sqrt(-1))\n"
                         "end" );
        }
        else
        {
            codeText = ( "begin\n"
                         "end" );
        }
    }

    QDomCDATASection data = m_XMLDocument.createCDATASection( codeText );
    m_codeSection.replaceChild( data, m_codeSection.firstChild() );


    foreach( cs8Variable * param, m_parameterModel->variableList() )
    {
        m_paramSection.appendChild( param->element() );
    }

    QString fileName_ = projectPath + fileName();
    QFile file( fileName_ );
    if ( !file.open( QIODevice::WriteOnly ) )
        return false;

    QTextStream stream( &file );
    stream.setCodec ("UTF-8");
    stream.setGenerateByteOrderMark (true);
    m_XMLDocument.save (stream,4,QDomNode::EncodingFromDocument);
    //stream << m_XMLDocument.toString();
    file.close();

    return true;
}

QString cs8Program::name() const
{
    //qDebug() << m_programSection.attribute("name");
    return m_programSection.attribute( "name" );
}

void cs8Program::setName( const QString &name )
{
    m_programSection.setAttribute( "name", QString( name ) );
}

QString cs8Program::toDocumentedCode()
{
    QString documentation;
    if ( !description().isEmpty() )
    {
        documentation = "\\brief\n";
        documentation += description() + "\n";
    }
    if ( !m_detailedDocumentation.isEmpty() )
    {
        documentation += "\\doc\n";
        documentation += m_detailedDocumentation+"\n";
    }

    documentation += m_parameterModel->toDocumentedCode();
    documentation += m_localVariableModel->toDocumentedCode();
    documentation += m_referencedGlobalVarModel->toDocumentedCode ();

    if (!m_applicationDocumentation.isEmpty())
    {
        documentation += "\\module\n";
        documentation += m_applicationDocumentation+"\n";
    }

    if (!m_mainPageDocumentation.isEmpty())
    {
        documentation += "\\mainpage\n";
        documentation += m_mainPageDocumentation+"\n";
    }

    if ( !m_tags.isEmpty() )
    {
        documentation += "\n";
        foreach( QString key, m_tags.uniqueKeys() )
        {
            foreach( QString value, m_tags.values( key ) )
            {
                documentation += QString( "\\%1 %2\n" )
                        .arg( key )
                        .arg( value );
            }
        }
    }

    if ( !m_copyRightMessage.isEmpty() )
    {
        documentation += "\n\\copyright\n";
        documentation += m_copyRightMessage+"\n";
    }


    if ( !documentation.isEmpty() )
        documentation += "\n";
    // format documentation section
    QStringList list;
    QRegExp rx;
    rx.setPattern( "\\b" );


    list = documentation.split( "\n" );
    list.replaceInStrings( QRegExp( "^" ), "  //" );
    documentation = list.join( "\n" ) + "\n";
    QString code = val3Code( false );
    rx.setPattern( "\\s*begin\\s*" );
    rx.setMinimal( true );
    int start = rx.indexIn( code, 0 );
    start += rx.matchedLength() + 1;

    code.insert( start++, documentation );

    tidyUpCode( code );
    return code;
}

QString cs8Program::definition() const
{

    return name() + "(" + m_parameterModel->toString() + ")";
}

void cs8Program::setCopyrightMessage( const QString &text )
{
    m_copyRightMessage = text;
    emit modified();
}

QString cs8Program::copyrightMessage() const
{
    return m_copyRightMessage;
}

