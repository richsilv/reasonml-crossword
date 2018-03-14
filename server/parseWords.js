const fs = require('fs');

function shuffle(a) {
  for (let i = a.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [a[i], a[j]] = [a[j], a[i]];
  }
  return a;
}

function parseWords(filename) {
  const contents = fs.readFileSync(filename).toString();
  return contents.split('\n').map((line, rank) => {
    const [_, word, count] = line.split('\t');
    return {
      word,
      count: parseInt(count, 0),
      rank,
      length: word ? word.length : 0
    };
  });
}

function makeDict(words) {
  const dict = {};

  words.forEach(word => {
    if (!word.length in dict) {
      dict[word.length] = [word.word];
    } else {
      dict[word.length].push[word.word];
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
          (thisWord.crossIndex >= word.start) &
            (thisWord.crossIndex <= word.start + word.length - 1)) ||
        ((word.start - 1 === thisWord.crossIndex ||
          word.start + word.length === thisWord.crossIndex) &&
          (word.crossIndex >= thisWord.start) &
            (word.crossIndex <= thisWord.start + thisWord.length - 1));
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

const [wordShape, grid] = makeGrid(10);
const dictionary = makeDict(parseWords('nouns.txt'));

function fitWords(wordShape) {
  const partialSolutions = [];
}

function findWordWithConstraints(dict, len, constraints) {
  const candidates = dict[len];
  const fitsTheseConstraints = fitsConstraints.bind(null, constraints);
  return candidates.find(fitsTheseConstraints);
}

function fitsConstraints(constraints, word) {
  return constraints.every(constraint => {
    word[constraint.letter] === constraint.value;
  });
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
