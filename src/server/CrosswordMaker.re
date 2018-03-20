type partialWord = {
  dir: Crossword.direction,
  col: int,
  row: int,
  start: int,
  crossIndex: int,
  length: int
};

let getRandomWord = (size: int) : partialWord => {
  let length = Random.int(size - 2) + 3;
  let start = Random.int(size - length);
  let crossIndex = Random.int(size);
  Random.bool() ?
    {dir: Crossword.Down, col: crossIndex, row: start, start, crossIndex, length} :
    {dir: Crossword.Right, col: start, row: crossIndex, start, crossIndex, length}
};

let checkWord = (word: partialWord, words: list(partialWord)): bool => {

};

let updateGrid = (word: partialWord, grid: Crossword.grid): Crossword.grid => {
  for (ind in 0 to word.length - 1) {
    if (word.dir === Crossword.Right) {
      grid[word.crossIndex][word.start + ind] = Crossword.Empty;
    } else {
      grid[word.start + ind][word.crossIndex] = Crossword.Empty;
    }
  }
  grid;
};

let makeGrid = (size: int) => {
  let grid = ref(Array.make_matrix(size, size, Crossword.FilledIn));
  let target = size;
  let count = ref(0);
  let words: ref(list(partialWord)) = ref([]);
  while (List.length(words^) < target) {
    count := count^ + 1;
    let thisWord = getRandomWord(size);
    if (checkWord(thisWord, words^)) {
      words := [thisWord, ...words^];
      grid := updateGrid(thisWord, grid^)
    }
  }
};
