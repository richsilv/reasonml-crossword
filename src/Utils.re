let fold_left_index = (f: ('a, 'b, int) => 'a, init: 'a, arr: array('b)) : 'a => {
  let res: ref('a) = ref(init);
  Array.iteri((ind, el) => res := f(res^, el, ind), arr);
  res^
};

let get_bounds_from_words = (_words: Crossword.words) : Crossword.coords =>
  _words
  |> Array.fold_left(
       (currentBounds: Crossword.coords, _word: Crossword.word) => {
         let wordBounds: Crossword.coords = {
           col:
             _word.start.col
             + (
               switch _word.dir {
               | Crossword.Down => 1
               | Crossword.Right => String.length(_word.answer)
               }
             ),
           row:
             _word.start.row
             + (
               switch _word.dir {
               | Crossword.Down => String.length(_word.answer)
               | Crossword.Right => 1
               }
             )
         };
         {col: max(wordBounds.col, currentBounds.col), row: max(wordBounds.row, currentBounds.row)}
       },
       {col: 0, row: 0}
     );

let make_empty_grid = (bounds: Crossword.coords) =>
  Array.make(bounds.row, None) |> Array.map((_) => Array.make(bounds.col, Crossword.Empty));

let get_grid_bounds = (vGrid: Crossword.grid) : Crossword.coords => {
  row: vGrid |> Array.length,
  col: vGrid[0] |> Array.length
};

let get_from_grid = (vGrid: Crossword.grid, vCoords: Crossword.coords) => vGrid[vCoords.row][vCoords.
                                                                    col];

let set_in_grid = (vGrid: Crossword.grid, vCoords: Crossword.coords, value: Crossword.contents) =>
  vGrid[vCoords.row][vCoords.col] = value;

let set_in_grid_safe =
    (vGrid: Crossword.grid, vCoords: Crossword.coords, value: Crossword.contents) => {
  let currentVal = vGrid[vCoords.row][vCoords.col];
  switch currentVal {
  | _ when Crossword.contents_equal(currentVal, value) => ()
  | Crossword.Empty => vGrid[vCoords.row][vCoords.col] = value
  | _ => raise(Arg.Bad("Value is inconsistent with existing grid"))
  }
};

let set_in_grid_overwrite =
    (vGrid: Crossword.grid, vCoords: Crossword.coords, value: Crossword.contents) => {
  let currentVal = vGrid[vCoords.row][vCoords.col];
  switch currentVal {
  | Crossword.Char(_)
  | Empty => vGrid[vCoords.row][vCoords.col] = value
  | FilledIn => raise(Arg.Bad("Value is inconsistent with existing grid"))
  }
};

let calc_grid_position =
    (start: Crossword.coords, dir: Crossword.direction, index: int)
    : Crossword.coords =>
  switch dir {
  | Crossword.Down => {col: start.col, row: start.row + index}
  | Right => {col: start.col + index, row: start.row}
  };

let build_full_grid = (vWords: Crossword.words) : Crossword.grid => {
  let vGrid = get_bounds_from_words(vWords) |> make_empty_grid;
  vWords
  |> Array.iter(
       (vWord: Crossword.word) =>
         vWord.answer
         |> String.iteri(
              (ind, letter) => {
                let letterCoords = calc_grid_position(vWord.start, vWord.dir, ind);
                set_in_grid_safe(vGrid, letterCoords, Char(letter))
              }
            )
     );
  vGrid
  |> Array.map(
       (vRow) =>
         vRow
         |> Array.map(
              (vContents) =>
                switch vContents {
                | Crossword.Char(c) => Crossword.Char(c)
                | _ => Crossword.FilledIn
                }
            )
     )
};

let empty_grid = (vGrid: Crossword.grid) : Crossword.grid => {
  let newGrid = vGrid |> get_grid_bounds |> make_empty_grid;
  vGrid
  |> Array.iteri(
       (rowNum, vRow) =>
         vRow
         |> Array.iteri(
              (colNum, vContents) =>
                switch vContents {
                | Crossword.FilledIn => newGrid[colNum][rowNum] = FilledIn
                | _ => newGrid[colNum][rowNum] = Empty
                }
            )
     );
  newGrid
};

let rec compareWords =
        (
          comparators: list(((Crossword.word, Crossword.word) => int)),
          wordA: Crossword.word,
          wordB: Crossword.word
        )
        : int =>
  switch comparators {
  | [] => 0
  | [comparator, ...rest] =>
    let result = comparator(wordA, wordB);
    switch result {
    | 0 => compareWords(rest, wordA, wordB)
    | x => x
    }
  };

let make_clues = (vWords: Crossword.words) : Crossword.clues => {
  let newWords = Array.copy(vWords);
  Array.sort(
    compareWords([
      (wordA, wordB) => wordA.start.row - wordB.start.row,
      (wordA, wordB) => wordA.start.col - wordB.start.col,
      (wordA, wordB) =>
        switch (wordA.dir, wordB.dir) {
        | (Right, Down) => (-1)
        | (Down, Right) => 1
        | _ => 0
        }
    ]),
    newWords
  );
  let (_, _, vClues) =
    Array.fold_left(
      (memo: (int, Crossword.coords, Crossword.clues), vWord: Crossword.word) => {
        let (clueNum, lastCoords, vClues) = memo;
        switch (vWord.start.row - lastCoords.row, vWord.start.col - lastCoords.col) {
        | (0, 0) => (clueNum, lastCoords, Array.append(vClues, [|(clueNum, vWord)|]))
        | _ => (clueNum + 1, vWord.start, Array.append(vClues, [|(clueNum + 1, vWord)|]))
        }
      },
      (0, {row: (-1), col: (-1)}, [||]: Crossword.clues),
      newWords
    );
  vClues
};

let rec array_find = (test: 'a => bool, arr: array('a)) : option('a) => {
  let arrayLength = arr |> Array.length;
  let thisEl = arr[0];
  let rest = Array.sub(arr, 1, arrayLength - 1);
  thisEl |> test ?
    Some(thisEl) :
    (
      switch arrayLength {
      | 1 => None
      | _ => array_find(test, rest)
      }
    )
};
