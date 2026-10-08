param([string]$elf, [int]$watch = 40)
$S = $PSScriptRoot
$log = "C:\Users\TwistZero\WoTM\build\pcsx2_test\log_play_$elf.txt"
Stop-Process -Name pcsx2-qt -Force -ErrorAction SilentlyContinue; Start-Sleep 2
& "$S\emu.ps1" start $elf | Out-Null
function Key($k, $wait) { $r = & "$S\emu.ps1" key $k 200; if ($r -notlike 'posted*') { throw "key $k failed: $r" }; Start-Sleep $wait }
function Fmv { $l = Select-String -Path $log -Pattern 'FMV (started|ended)' -ErrorAction SilentlyContinue | Select-Object -Last 1; return ($l -and $l.Line -match 'started') }
# 1. skip every cutscene as soon as it starts (Enter = Start); stop when none ran for 8 s
$quiet = 0; $skipped = 0; $t0 = Get-Date
while ($quiet -lt 8 -and ((Get-Date) - $t0).TotalSeconds -lt 90) {
  if (Fmv) { Key 'ENTER' 1; $skipped++; $quiet = 0 } else { Start-Sleep 1; if ($skipped -gt 0) { $quiet++ } }
}
"skipped $skipped cutscene key presses"
# 2. title -> 1 player -> free-for-all -> defaults -> monsters -> stage
Key 'ENTER' 3; Key 'X' 3; Key 'DOWN' 1; Key 'X' 3; Key 'X' 3; Key 'X' 3; Key 'X' 3; Key 'X' 3; Key 'X' 0
$bad = $null; $inLevel = $false
for ($i = 0; $i -lt $watch; $i++) {
  Start-Sleep 2
  if (-not (Get-Process pcsx2-qt -ErrorAction SilentlyContinue)) { $bad = 'emulator exited'; break }
  $m = Select-String -Path $log -Pattern 'Unrecognized op|ReportErrorAsync|Trap exception' | Select-Object -First 1
  if ($m) { $bad = $m.Line; break }
}
& "$S\emu.ps1" snap "$S\play_$elf.png"
$rp = (Select-String -Path $log -Pattern 'Rich presence' | Select-Object -Last 1).Line
"last: $rp"
if ($bad) { "FAIL: $bad" } else { "OK: no error in ${watch}x2 s after the stage was chosen" }
