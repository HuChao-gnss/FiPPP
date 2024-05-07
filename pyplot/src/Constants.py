#
# This py file stores all common constants
#
from datetime import datetime

ELE = "elevation"
AZI = "azimuth"
NSAT = 'numSat'
DATE_TIME = 'DateTime'
E = 'E'
N = 'N'
U = 'U'
FLAG = 'flag'
VALUE = 'value'
SPLIT = "\\"

FIRST = 1
SECOND = 2
THIRD = 4
FOURTH = 8
FIFTH = 16

BEGIN_TIME = datetime(1999, 11, 3, 0, 0, 0)

Types = {
    1: [
        '1G', 'C12G', 'C125G',
        '1E', 'C15E', 'C157E', 'C1578E', 'C15786E',
        '2C', 'C26C', 'C261C', 'C2615C', 'C26158C',
        '1C', 'C15C', 'C152C', 'C1526C', 'C15268C',
        'C15C', 'C158C'
    ],
    2: [
        '2G', 'C15G',
        '5E', 'C17E',
        '6C', 'C27C', 'C21C',
        '5C', 'C12C',
        'C26C'
    ],

    4: [
        '5G',
        '7E', 'C18E',
        '6C', 'C25C',
        '5C', 'C16C'
    ],

    8: [
        '8E', 'C16E',
        '5C', 'C25C',
        '6C', 'C16C'
    ],

    16: [
        '6E',
        '8C'
    ],

}
