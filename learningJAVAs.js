let randomNumber = Math.floor(Math.random() * 100) + 1;
let attempts = 0;

const guessInput = document.getElementById('guessInput');
const submitGuessButton = document.getElementById('submitGuess');
const resultDiv = document.getElementById('result');
const resetGameButton = document.getElementById('resetGame');


submitGuessButton.addEventListener('click', function() {
    const userGuess = Number(guessInput.value);
    attempts++;

    
    resultDiv.className = '';

    if (userGuess < 1 || userGuess > 100 || isNaN(userGuess)) {
        resultDiv.textContent = "Please enter a number between 1 and 100.";
    } else if (userGuess < randomNumber) {
        resultDiv.textContent = "Too low! Try again.";
        resultDiv.classList.add('too-low'); 
    } else if (userGuess > randomNumber) {
        resultDiv.textContent = "Too high! Try again.";
        resultDiv.classList.add('too-high'); 
    } else {
        resultDiv.textContent = `Congratulations! You've guessed the number ${randomNumber} in ${attempts} attempts.`;
        resultDiv.classList.add('correct');
        resetGameButton.style.display = 'inline';
        submitGuessButton.disabled = true;
    }
    guessInput.value = '';
});


resetGameButton.addEventListener('click', function() {
    randomNumber = Math.floor(Math.random() * 100) + 1;
    attempts = 0;
    resultDiv.textContent = '';
    resultDiv.className = '';
    guessInput.value = '';
    resetGameButton.style.display = 'none';
    submitGuessButton.disabled = false;
});
