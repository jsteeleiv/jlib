from random import randint

def rocpapscidyn():
    



def greet(state):
    return print("Let's play a nice game of Rock/Paper/Scissors") or "nice"

def play(state):
    match randint(1, 3):
        case 1: return print("I choose rock") or "rock"
        case 2: return print("I choose paper") or "paper"
        case 3: return print("I choose scissors") or "scissors"

def judge(state):
    play_pair = state.get("my_play"), state.get("your_play")
    options = "rock", "paper", "scissors"
    ## Create pairs of options such as [(o1, o2), (o2, o3), (o3, o1)]
    loss_pairs = [(o1, o2) for o1, o2 in zip(options, options[1:] + options[:1])]
    ## Create pairs of options such as [(o2, o1), (o3, o2), (o1, o3)]
    win_pairs  = [(o2, o1) for o1, o2 in loss_pairs]
    if play_pair in loss_pairs:
        return print("I lost :(") or "user_wins"
    if play_pair in win_pairs:
        return print("I win :)") or "ai_wins"
    return print("It's a tie!") or "everyone_wins"

state = {}
state["my_tone"] = greet(state)
state["my_play"] = play(state)
state["your_play"] = input("Your Play").strip() or print("You Said: ", end="") or play(state)
state["result"] = judge(state)

print(state)