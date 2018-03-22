const fs = require('fs');
const path = require('path');

const MAX_TIMES_FITTED = 10;

function shuffle(a) {
  for (let i = a.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [a[i], a[j]] = [a[j], a[i]];
  }
  return a;
}

// if words are in word frequency pairs
function parseWords(filename) {
  if (filename instanceof Array) {
    return filename.reduce((allWords, thisFilename) => {
      return allWords.concat(parseWords(thisFilename));
    }, []);
  }
  const contents = fs.readFileSync(filename).toString();
  return contents
    .split('\n')
    .map((line, rank) => {
      if (!line) return null;
      let word, count;
      const [a, b, c] = line.split('\t');
      if (!b) {
        word = a;
        count = 1;
      } else {
        word = b;
        count = c;
      }
      return {
        ...reduceWord(word),
        count: parseInt(count, 0),
        rank,
        length: word ? word.length : 0
      };
    })
    .filter(wordObj => !!wordObj);
}

function reduceWord(word) {
  let wordPos = 0,
    reducedWord = '',
    wordBreaks = [];
  Array.from(word.toUpperCase()).forEach(letter => {
    if (letter === '_') {
      wordBreaks.push(wordPos);
    } else {
      wordPos += 1;
      reducedWord = reducedWord + letter;
    }
  });
  return { word: reducedWord, wordBreaks };
}

const BAD_WORD_REGEX = /[0-9\-\.\,\'&]/;

// just a list of words
function loadWords(filename) {
  const contents = fs.readFileSync(filename).toString();
  return contents
    .split('\n')
    .map(word => {
      if (BAD_WORD_REGEX.test(word)) return null;
      return {
        word: word.toUpperCase(),
        length: word ? word.length : 0
      };
    })
    .filter(word => word);
}

function makeDict(words) {
  const dict = {};

  words.forEach(word => {
    if (!(word.length in dict)) {
      dict[word.length] = [word.word];
    } else {
      dict[word.length].push(word.word);
    }
  });

  for (len of Object.keys(dict)) {
    shuffle(dict[len]);
  }

  return dict;
}

function makeGrid(size) {
  const grid = new Array(size).fill(0).map(() => new Array(size).fill(0));

  const words = [];
  const target = size;
  let count = 0;

  while (words.length < target && count < 100000) {
    count++;
    const word = getRandomWord(size);
    if (checkWord(word, words)) {
      words.push(word);
      updateGrid(word, grid);
    }
  }

  if (words.length < target) return makeGrid(size);

  return [words, grid];
}

function getRandomWord(gridSize) {
  const length = Math.floor(Math.random() * (gridSize - 2)) + 3;
  const start = Math.floor(Math.random() * (gridSize - length));
  const crossIndex = Math.floor(Math.random() * gridSize);
  return Math.random() < 0.5
    ? {
        dir: 'D',
        col: crossIndex,
        row: start,
        start,
        crossIndex,
        length
      }
    : {
        dir: 'R',
        col: start,
        row: crossIndex,
        start,
        crossIndex,
        length
      };
}

function checkWord(word, words) {
  const intersectsWord = intersects.bind(null, word);
  if (words.length > 0 && !words.some(intersectsWord)) {
    // console.log('Fails intersection check');
    return false;
  }

  return !words.find(thisWord => {
    if (thisWord.dir === word.dir) {
      // check words aren't next to each other in the same direction
      const nextTo =
        (thisWord.start + thisWord.length - 1 >= word.start ||
          word.start + word.length - 1 >= thisWord.start) &&
        (thisWord.crossIndex - word.crossIndex <= 1 &&
          thisWord.crossIndex - word.crossIndex >= -1);
      // if (nextTo) console.log('Fails next to check');
      return nextTo;
    } else {
      // check words don't butt up against each other in perpendicular directions
      const buttingUp =
        ((thisWord.start - 1 === word.crossIndex ||
          thisWord.start + thisWord.length === word.crossIndex) &&
          thisWord.crossIndex >= word.start &&
          thisWord.crossIndex <= word.start + word.length - 1) ||
        ((word.start - 1 === thisWord.crossIndex ||
          word.start + word.length === thisWord.crossIndex) &&
          word.crossIndex >= thisWord.start &&
          word.crossIndex <= thisWord.start + thisWord.length - 1);
      // if (buttingUp) console.log('Fails butting up check');
      return buttingUp;
    }
  });
}

function updateGrid(word, grid) {
  new Array(word.length).fill(0).forEach((_, ind) => {
    if (word.dir === 'R') {
      grid[word.crossIndex][word.start + ind] = 1;
    } else {
      grid[word.start + ind][word.crossIndex] = 1;
    }
  });
}

function fullyConnected(words) {
  return words.every((word, ind) => {
    return words.some((thisWord, thisInd) => {
      return ind !== thisInd && intersects(word, thisWord);
    });
  });
}

function intersects(wordA, wordB) {
  if (wordA.dir === wordB.dir) return false;

  return (
    wordA.start <= wordB.crossIndex &&
    wordA.start + wordA.length - 1 >= wordB.crossIndex &&
    wordB.start <= wordA.crossIndex &&
    wordB.start + wordB.length - 1 >= wordA.crossIndex
  );
}

function fitWords(wordShape, dict) {
  const partialSolutions = [{ fittedWords: [], reject: [], timesFitted: 0 }];
  let currentWordInd = 0;
  let latestSolution = partialSolutions[currentWordInd];

  while (currentWordInd < wordShape.length) {
    console.log(`Fit ${currentWordInd} words so far.`);
    const thisWordShape = wordShape[currentWordInd];
    const { fittedWords, reject, timesFitted } = latestSolution;
    const constraints = calcConstraints(thisWordShape, fittedWords);
    const nextWord = findWordWithConstraints(
      dict,
      thisWordShape.length,
      constraints,
      reject
    );
    if (nextWord && timesFitted < MAX_TIMES_FITTED) {
      latestSolution.timesFitted += 1;
      latestSolution = {
        fittedWords: latestSolution.fittedWords.concat(
          Object.assign({}, thisWordShape, { value: nextWord })
        ),
        reject: [],
        timesFitted: 0
      };
      partialSolutions.push(latestSolution);
      currentWordInd += 1;
    } else {
      if (timesFitted >= MAX_TIMES_FITTED)
        console.log(`Rejecting due to failure after ${timesFitted} failures.`);
      const rejectWord = fittedWords.slice(-1)[0];
      if (!rejectWord) {
        throw new Error('No solution can be found');
      }
      partialSolutions.pop();
      currentWordInd -= 1;
      latestSolution = partialSolutions[currentWordInd];
      latestSolution.reject.push(rejectWord.value);
    }
  }

  return latestSolution.fittedWords;
}

function findWordWithConstraints(dict, len, constraints, reject) {
  const candidates = dict[len];
  const fitsTheseConstraints = fitsConstraints.bind(null, constraints, reject);
  return candidates.find(fitsTheseConstraints);
}

function fitsConstraints(constraints, reject, word) {
  return (
    reject.indexOf(word) === -1 &&
    constraints.every(
      constraint => word[constraint.letter] === constraint.value
    )
  );
}

function calcConstraints(wordShape, fittedWords) {
  return fittedWords
    .map(fittedWord => {
      if (fittedWord.dir === wordShape.dir) return undefined;
      if (
        fittedWord.crossIndex < wordShape.start ||
        fittedWord.crossIndex > wordShape.start + wordShape.length - 1
      ) {
        return undefined;
      }
      const value = fittedWord.value[wordShape.crossIndex - fittedWord.start];
      if (!value) return undefined;
      return {
        letter: fittedWord.crossIndex - wordShape.start,
        value
      };
    })
    .filter(constraint => !!constraint);
}

const [wordShape, grid] = makeGrid(12);
// const dictionary = makeDict(loadWords(__dirname + '/words.txt'));
const wordFiles = [
  'adjectives.txt',
  'adverbs.txt',
  'nouns.txt',
  'verbs.txt',
  'countries.txt',
  'englishCounties.txt',
  'usStates.txt',
  'usStateCapitals.txt',
  'nationalCapitals.txt'
];
const dictionary = makeDict(
  parseWords(
    wordFiles.map(file => path.join(__dirname, '..', 'wordData', file))
  )
);

console.log(fitWords(wordShape, dictionary));
