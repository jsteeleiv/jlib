from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from typing import Mapping as jmap
from typing import TypeVar as tvar
import subprocess as subp
import schedule as sched
#from typing import Dict
import platform as plat
import datetime as dt
import random as rand
import smtplib
import shutil
import time
import sys
import os


K = tvar("K")
V = tvar("V")

def me():
    print("jsteele@ecpi.edu\nAKA:\tThe student formerly known as judste2266\n")

def age():
    userbd = input("What is your birthday? (mm/dd/yyyy): ")
    mn, dy, yr = userbd.split("/")
    bd = dt.datetime(int(yr), int(mn), int(dy))
    d = dt.datetime.now()
    age = d - bd
    print("Days Alive: " + str(age.days))
    print("Years: " + str(age.days/365.25))

def is_even(x):
    return not (x & 1)

def cwd(arg=1):
    cwd = os.getcwd()
    if arg:
        print(cwd)
    else:
        return cwd

def specs():
    opsys = plat.platform()
    systype = plat.machine()
    sysname = plat.node()
    sysproc = plat.processor()
    sysedit = plat.win32_edition()

    print(f"OS: {opsys}")
    print(f"Type: {systype}")
    print(f"Name: {sysname}")
    print(f"Edition: {sysedit}")
    print(f"Processor: {sysproc}")
    

def nl(arg=1):
    for i in range(arg):
        print("")

def note(path, arg):
    status = os.path.exists(path) 
    if status:
        f = open(f"{path}", "a")
        f.write(f"{arg}\n")
        f.close()
    else:
        f = open(f"{path}", "w")
        f.write(f"{arg}\n")
        f.close()

    p = subp.Popen(f"notepad {path}")

def open_mspaint(file):
    p = subp.Popen(f"mspaint {file}")


# def f(**kwargs): ...
# f(**my_dict)

# def hash_str_md5(str_):
#     hash_obj = hashlib.md5()
#     hash_obj.update(str_.encode('utf-8'))
#     return hash_obj.hexdigest()  # hex representation of hash



def dict_zip(d1: jmap[K, V], d2: jmap[K, V])  -> dict[K, V]:
    return dict(d1) | dict(d2)

def dict_zip_splat(d1: dict, d2: dict) -> dict:
    return {**d1, **d2}

## == FILE Handling ==

def touch(path):
    os.utime(path, None) if os.path.exists(path) else open(path, 'w').close()  

def isdir(path):
    meta = {}
    if os.path.exists(path) and os.path.isdir(path):
        print("this")

def mkdir(path, mod=0o755, p=False, n=True, t=False, a=False, x=False, v=False):
    kwargs = [('mod', mod), ('p', p), ('n', n), ('t', t), ('a', a), ('x', x), ('v', v)]
    verbose = {}
    log = lambda task, status: verbose.update({task: status})
    for k, v in kwargs:
        match k:
            case 'p' if p:
                os.makedirs(path, mode=mod, exist_ok=True)
                log("p", "Created full, necessary path")
                pass
            case 'n' if n:
                # TODO Do something for 'v'
                pass
            case 't' if t:
                # TODO Do something for 'n'
                pass
            case 'a' if a:
                # TODO Do something for 'x'
                pass
            case 'x' if v:
                # TODO Do something for 'x'
                pass
            case _:
                os.mkdir(path)

def read_file_as_bytes(path):
    # bytes_dict = read_file_as_bytes('path/to/file')
    bytes_ = {}
    byte = 0
    with open(path, 'rb') as file:
        for chunk in iter(lambda: file.read(8), b""):
            bytes_[byte] = chunk
            byte += 1
    return bytes_

def read_file_as_pages(path):
    # pages ({}}), aligned (?), bytes_ = read_file_as_pages('path/to/file')
    PAGE = 4096
    pages = {}
    page = 0
    aligned = True
    bytes_ = 0
    with open(path, 'rb') as file:
        for chunk in iter(lambda: file.read(PAGE), b""):
            pages[page] = chunk
            page += 1
        if len(chunk) != PAGE:
            aligned = False
            bytes_ = PAGE - len(chunk)

    return pages, aligned, bytes_

def make_list_rng(cap=100, sort=False):
    lst = [cap]
    for idx, val in enumerate(lst):
        lst.append(rand.randint(1, 10))

    if sort:
        return lst.sort()

    return lst
    


## == MISC Utilities ==

# wifi function ... ? forgot why I wrote this ...
def do_wifi():
    results = {}
    data=subp.check_output(['netsh', 'wlan', 'show', 'profiles']).decode('utf-8', errors="backslashreplace").split("\n")
    profiles = [i.split(":")[1][1:-1] for b in results if "Key Content" in b]
    for i in profiles:
        try:
            results = subp.check_output(['netsh', 'wlan', 'show', 'profile', 'i', 'key=clear']).decode('utf-8', errors="backslashreplace").split("\n")
            results = [b.split(":")[1][1:-1] for b in results if "Key Content" in b]
            try:
                print("{:<30}| {:<}".format(i, results[0]))
            except IndexError:
                print("{:<30}| {:<}".format(i, ""))
        except subp.CalledProcessError:
            print("{:<30}| {:<}".format(i, "ENCODING ERROR"))
    input("")

    return results


# Display process bar in the terminal
def progress(current, total=1, pad=10, fill="=", 
             blank=" ", nested="False", color="reset"):
    """
    current: current process/process iteration count
    total: total iterations to complete
    pad: total padding (percentage) to allow around bar
    fill: symbol to use as the process filler
    blank: symbol to use as the unfilled area
    """
    width = shutil.get_terminal_size().columns
    length = width - (padding := int(width * (pad / 100)))
    process = (current + 1)/total
    symbol = fill * int(round(process * length))
    space = blank * (length - len(symbol))
    percent = int(round(process * 100, 1))
    color = "cyan" if nested else "reset"
    sys.stdout.write(f"\r[{hues[color]}{symbol}{space}{hues["reset"]}] {hues[color]}{percent}%{hues[color]}{hues["reset"]}")
    sys.stdout.write(hues["reset"])
    sys.stdout.flush()

def send_daily_report():



    # Your Gmail credentials
    gmail_user = 'your_email@gmail.com'
    gmail_password = 'your_app_password'  # Use the App Password you generated earlier

    # Create a message
    msg = MIMEMultipart()
    msg['From'] = gmail_user
    msg['To'] = 'recipient@example.com'  # Replace with the recipient's email address
    msg['Subject'] = 'Daily Report'

    # Body of the email
    message = 'This is your daily report email.'
    msg.attach(MIMEText(message, 'plain'))

    try:
        # Connect to Gmail's SMTP server
        server = smtplib.SMTP('smtp.gmail.com', 587)
        server.starttls()
        server.login(gmail_user, gmail_password)

        # Send the email
        server.sendmail(gmail_user, msg['To'], msg.as_string())
        server.quit()
        print('Email sent successfully')
    except Exception as e:
        print(f'Error: {str(e)}')


def do_the_thing():
    # Schedule the script to run daily at a specific time (adjust as needed)
    sched.every().day.at('08:00').do(send_daily_report)

    # Run the script continuously
    while True:
        sched.run_pending()
        time.sleep(1)


## == COLORS ==
hues = {
    'reset': "\033[0m",  # Reset all attributes

    'black': "\033[30m",
    'red': "\033[31m",
    'green': "\033[32m",
    'yellow': "\033[33m",
    'blue': "\033[34m",
    'magenta': "\033[35m",
    'cyan': "\033[36m",
    'white': "\033[37m",

    # Background colors
    'bg_black': "\033[40m",
    'bg_red': "\033[41m",
    'bg_green': "\033[42m",
    'bg_yellow': "\033[43m",
    'bg_blue': "\033[44m",
    'bg_magenta': "\033[45m",
    'bg_cyan': "\033[46m",
    'bg_white': "\033[47m",

    # Bright foreground colors
    'br_black': "\033[90m",
    'br_red': "\033[91m",
    'br_green': "\033[92m",
    'br_yellow': "\033[93m",
    'br_blue': "\033[94m",
    'br_magenta': "\033[95m",
    'br_cyan': "\033[96m",
    'br_white': "\033[97m",

    # Bright background colors
    'bg_br_black': "\033[100m",
    'bg_br_red': "\033[101m",
    'bg_br_green': "\033[102m",
    'bg_br_yellow': "\033[103m",
    'bg_br_blue': "\033[104m",
    'bg_br_magenta': "\033[105m",
    'bg_br_cyan': "\033[106m",
    'bg_br_white': "\033[107m",

    # Formatting options
    'bold': "\033[1m",
    'underline': "\033[4m",
    'reversed': "\033[7m",
    'hidden': "\033[8m"
}
