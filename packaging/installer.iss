#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define Year GetDateTimeString("yyyy","","")

[Setup]
AppName=Wahmbulance
OutputBaseFilename=Wahmbulance-{#Version}-Windows
AppCopyright=Copyright (C) {#Year} Chris Woodall
AppPublisher=Chris Woodall
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3"
DisableStartupPrompt=yes

[Files]
source: "{src}..\Builds\Wahmbulance_artefacts\Release\VST3\Wahmbulance.vst3\*.*"; DestDir: "{commoncf64}\VST3\Wahmbulance.vst3\"; Check: Is64BitInstallMode; Flags: external overwritereadonly ignoreversion; Attribs: hidden system;
