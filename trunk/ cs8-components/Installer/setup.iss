; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Val3 Tools"
#define MyAppVersion "1.5"
#define MyAppPublisher "SAXE Swiss System AB"
#define MyAppURL "http://www.swiss-system.se"
#define QT "D:\QT\QtSDK\Desktop\Qt\4.8.1\mingw"
#define MINGW "C:\MinGW"


[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{22A102F5-E18C-421B-8A0C-80C01031AAFA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Saxe Swiss System Val3 Tools
DefaultGroupName={#MyAppName}
OutputDir=D:\data\Cpp\cs8-components\Installer
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "D:\data\Cpp\_SAXESwissSystem\_Libraries\cs8-components\cs8ProgramComponent\bin\extractPrototype.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\data\Cpp\_SAXESwissSystem\_Libraries\cs8-components\cs8ProgramComponent\bin\Val3 Documentation Editor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\data\Cpp\_SAXESwissSystem\_Libraries\cs8-components\cs8ProgramComponent\bin\val3Check.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\data\Cpp\_SAXESwissSystem\_Libraries\cs8-components\cs8ProgramComponent\bin\val3CreateAPI.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\data\Cpp\_SAXESwissSystem\_Libraries\cs8-components\cs8ProgramComponent\bin\extractDoc.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MINGW}\bin\mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MINGW}\bin\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MINGW}\bin\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\QtXml4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\QtNetwork4.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Val3 Documentation Editor"; Filename: "{app}\Val3 Documentation Editor.exe"
Name: "{group}\Val3 Create API"; Filename: "{app}\val3CreateAPI.exe"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

