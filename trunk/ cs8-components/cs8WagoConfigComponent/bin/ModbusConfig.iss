; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Modbus Config Tool
AppVerName=Modbus Config Tool 1.1
AppPublisher=Swiss-System AB
AppPublisherURL=http://www.swiss-system.se
AppSupportURL=http://www.swiss-system.se
AppUpdatesURL=http://www.swiss-system.se
DefaultDirName={pf}\SwissSystem\Modbus Config Tool
DefaultGroupName=Swiss System\Modbus Config Tool
OutputBaseFilename=ModbusConfigSetup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "d:\Data\Cpp\components\cs8WagoConfigComponent\bin\ModbusConfig.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\4.3.2\bin\QtXml4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\4.3.2\bin\QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\4.3.2\bin\QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\4.3.2\bin\QtNetwork4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\MinGW\bin\mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Modbus Config Tool"; Filename: "{app}\ModbusConfig.exe"
Name: "{commondesktop}\Modbus Config Tool"; Filename: "{app}\ModbusConfig.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\ModbusConfig.exe"; Description: "{cm:LaunchProgram,Modbus Config Tool}"; Flags: nowait postinstall skipifsilent
