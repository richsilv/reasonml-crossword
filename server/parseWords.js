const fs = require('fs');

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
  // if (!fullyConnected(words)) return makeGrid(size);

  return [words, grid];
}

function getRandomWord(gridSize) {
  const length = Math.floor(Math.random() * (gridSize - 3)) + 3;
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
  return !words.find(thisWord => {
    return (
      // Need to check that two words don't but up against each other
      thisWord.dir === word.dir &&
      (thisWord.start + thisWord.length >= word.start ||
        word.start + word.length >= thisWord.start) &&
      (thisWord.crossIndex - word.crossIndex <= 1 &&
        thisWord.crossIndex - word.crossIndex >= -1)
    );
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
    wordA.start + wordA.end >= wordB.crossIndex &&
    wordB.start <= wordA.crossIndex &&
    wordB.start + wordB.end >= wordA.crossIndex
  );
}

console.log(makeGrid(10));
