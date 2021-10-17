# CPPFCAF - C Plus Plus the Fastest COMES Archive Format
CAF dekoder. Proudly written in c++ and opimized for speed.

| Command | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `../uncaf ../../test.caf .` | 18.4 ± 0.6 | 17.5 | 23.0 | 1.02 ± 0.04 |
| `../dec ../../test.caf` | 18.0 ± 0.4 | 17.1 | 19.3 | 1.00 |
| `python ../uncaffer.py ../../test.caf` | 361.8 ± 2.0 | 358.3 | 365.5 | 20.07 ± 0.48 |



| Command | Mean [ms] | Min [ms] | Max [ms] | Relative |
|:---|---:|---:|---:|---:|
| `../uncaf ../../bydgoszcz.caf .` | 187.7 ± 1.1 | 186.0 | 190.2 | 1.12 ± 0.02 |
| `../dec ../../bydgoszcz.caf` | 167.7 ± 2.7 | 165.4 | 176.6 | 1.00 |

uncaf by [liquidev](https://github.com/liquidev)
uncaffer.py by [Firstbober](https://github.com/Firstbober/)

## Usage
``./dec  archive.caf ``

makes archive.caf directory with files in working directory
## CAF
Standard (PL only) [caf](https://github.com/comes-group/standards/blob/master/polish/caf.md).
# Made by KuczaRacza
