type partialWord = {
  dir: Crossword.direction,
  col: int,
  row: int,
  start: int,
  crossIndex: int,
  length: int
};

type completedWord = {
  dir: Crossword.direction,
  col: int,
  row: int,
  start: int,
  crossIndex: int,
  length: int,
  value: string
};

let getRandomWord = (size: int) : partialWord => {
  let length = Random.int(size - 2) + 3;
  let start = Random.int(size - length);
  let crossIndex = Random.int(size);
  Random.bool() ?
    {dir: Crossword.Down, col: crossIndex, row: start, start, crossIndex, length} :
    {dir: Crossword.Right, col: start, row: crossIndex, start, crossIndex, length}
};

let intersects = (wordA: partialWord, wordB: partialWord) : bool =>
  wordA.dir === wordB.dir ?
    true :
    wordA.start <= wordB.crossIndex
    && wordA.start
    + wordA.length
    - 1 >= wordB.crossIndex
    && wordB.start <= wordA.crossIndex
    && wordB.start
    + wordB.length
    - 1 >= wordA.crossIndex;

let checkWord = (word: partialWord, words: list(partialWord)) : bool => {
  let intersectsWord = intersects(word);
  List.length(words) > 0 && ! List.exists(intersectsWord, words) ?
    false :
    !
      List.exists(
        (thisWord: partialWord) =>
          thisWord.dir === word.dir ?
            (
              thisWord.start
              + thisWord.length
              - 1 >= word.start
              || word.start
              + word.length
              - 1 >= thisWord.start
            )
            && thisWord.crossIndex
            - word.crossIndex <= 1
            && thisWord.crossIndex
            - word.crossIndex >= (-1) :
            (
              thisWord.start
              - 1 === word.crossIndex
              || thisWord.start
              + thisWord.length === word.crossIndex
            )
            && thisWord.crossIndex >= word.start
            && thisWord.crossIndex <= word.start
            + word.length
            - 1
            || (
              word.start
              - 1 === thisWord.crossIndex
              || word.start
              + word.length === thisWord.crossIndex
            )
            && word.crossIndex >= thisWord.start
            && word.crossIndex <= thisWord.start
            + thisWord.length
            - 1,
        words
      )
};

let updateGrid = (word: partialWord, grid: Crossword.grid) : Crossword.grid => {
  for (ind in 0 to word.length - 1) {
    if (word.dir === Crossword.Right) {
      grid[word.crossIndex][word.start + ind] = Crossword.Empty
    } else {
      grid[word.start + ind][word.crossIndex] = Crossword.Empty
    }
  };
  grid
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

type letterConstraint = {
  letter: int,
  value: char
};

let calcConstraints = (wordShape: partialWord, fittedWords: list(completedWord)): list(letterConstraint) => {
  fittedWords |> List.fold_left((letterConstraints, fittedWord): list(letterConstraint) => {
    fittedWord.dir === wordShape.dir ||
    fittedWord.crossIndex < wordShape.start ||
    fittedWord.crossIndex > wordShape.start + wordShape.length - 1 ||
    wordShape.crossIndex - fittedWord.start < 0 ||
    wordShape.crossIndex - fittedWord.start >= String.length(fittedWord.value)
      ? letterConstraints
      : {
        let value = wordShape.crossIndex - fittedWord.start |> String.get(fittedWord.value);
        [{ letter: fittedWord.crossIndex - wordShape.start, value: value }, ...letterConstraints];
      }
  }, []);
};
