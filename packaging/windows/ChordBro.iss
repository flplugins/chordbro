#define AppName "ChordBro"
#define AppVersion "3.0.0"
#define AppPublisher "FL Plugins"
#define PluginSource "..\..\ripchord-main\build\ChordBro_artefacts\Release\VST3\ChordBro.vst3"
#define PremiumSource "..\..\commercial-content\premium-packs"

[Setup]
AppId={{6CE563F9-AFB5-4C1E-8FD0-CB4D9B77FC73}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={autopf}\FL Plugins\ChordBro
DefaultGroupName=FL Plugins\ChordBro
OutputDir=..\dist\windows
OutputBaseFilename=ChordBro-Setup-{#AppVersion}
Compression=lzma2/max
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
PrivilegesRequired=admin
UninstallDisplayIcon={app}\chordbro.txt

[Dirs]
Name: "{commonappdata}\FL Plugins\ChordBro\Premium Packs"

[InstallDelete]
Type: files; Name: "{commonappdata}\FL Plugins\ChordBro\Premium Packs\dark-cinema\presets\ChordBro *.rpc"
Type: files; Name: "{commonappdata}\FL Plugins\ChordBro\Premium Packs\essentials\presets\ChordBro *.rpc"
Type: files; Name: "{commonappdata}\FL Plugins\ChordBro\Premium Packs\gospel-colors\presets\ChordBro *.rpc"
Type: files; Name: "{commonappdata}\FL Plugins\ChordBro\Premium Packs\rb-velvet\presets\ChordBro *.rpc"
Type: files; Name: "{commonappdata}\FL Plugins\ChordBro\Premium Packs\trap-vault\presets\ChordBro *.rpc"

[Files]
Source: "{#PluginSource}\*"; DestDir: "{commoncf64}\VST3\ChordBro.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "factory-seeding.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PremiumSource}\*"; DestDir: "{commonappdata}\FL Plugins\ChordBro\Premium Packs"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\ChordBro Release Notes"; Filename: "{app}\chordbro.txt"

[Run]
Filename: "{cmd}"; Parameters: "/C echo ChordBro installs the official VST3, seeds the factory library on first launch, and preloads all premium preset packs. > ""{app}\chordbro.txt"""; Flags: runhidden
