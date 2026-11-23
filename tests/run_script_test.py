"""Check runner sequencing without touching a serial device or real Arduino CLI."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

runner = Path(__file__).resolve().parents[1] / 'run.sh'
with tempfile.TemporaryDirectory(prefix='liahona runner ') as folder:
    root = Path(folder)
    shutil.copy2(runner, root / 'run.sh')
    (root / 'bin').mkdir()
    cli = root / 'bin' / 'arduino-cli'
    cli.write_text('''#!/bin/sh
printf '%s\\n' "$*" >> "$TEST_RUN_LOG"
if [ "$1" = "${TEST_FAIL_STAGE:-none}" ]; then exit 7; fi
''')
    cli.chmod(0o755)
    for fail, expected in [('none', ['compile', 'upload', 'monitor']),
                           ('compile', ['compile']), ('upload', ['compile', 'upload'])]:
        log = root / 'calls.txt'
        log.unlink(missing_ok=True)
        env = dict(os.environ, TEST_RUN_LOG=str(log), TEST_FAIL_STAGE=fail)
        result = subprocess.run(['sh', str(root / 'run.sh'), '/dev/test-nano'], env=env)
        lines = log.read_text().splitlines()
        assert [line.split()[0] for line in lines] == expected
        assert (result.returncode == 0) == (fail == 'none')
        assert 'arduino:avr:nano:cpu=atmega328old' in lines[0]
        if len(lines) > 1:
            assert '--port /dev/test-nano' in lines[1]
        if len(lines) > 2:
            assert '--config baudrate=115200' in lines[2]
print('runner tests passed: sequencing, quoted project paths, build/upload failures')
