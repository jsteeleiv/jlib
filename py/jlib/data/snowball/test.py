from snowball import Snowball

ball = Snowball(initial=10)

def accumulate(current, observation):
    return current + observation

ball.roll(5, accumulate)
ball.roll(3, accumulate)

print("Present:", ball.present)
print("Iteration:", ball.iteration)
print("Past:", ball.past)
