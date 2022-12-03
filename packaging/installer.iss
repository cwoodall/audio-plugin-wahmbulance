#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define Year GetDateTimeString("yyyy","","")

[Setup]
AppName=Autowah
OutputBaseFilename=Autowah-{#Version}-Windows
AppCopyright=Copyright (C) {#Year} Melatonin
AppPublisher=Melatonin
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3"
DisableStartupPrompt=yes

[Files]
source: "{src}..\Builds\Autowah_artefacts\Release\VST3\Autowah.vst3\*.*"; DestDir: "{commoncf64}\VST3\Autowah.vst3\"; Check: Is64BitInstallMode; Flags: external overwritereadonly ignoreversion; Attribs: hidden system;
