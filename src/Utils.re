let fold_left_index = (f: ('a, 'b, int) => 'a, init: 'a, arr: array('b)): 'a => {
  let res: ref('a) = ref(init);
  Array.iteri(
    (ind, el) => {
      res := f(res^, el, ind);
    },
    arr
  );
  res^;
};

type coords = {
  row: int,
  col: int
};

type direction =
  | Down
  | Right;

type word = {
  start: coords,
  dir: direction,
  hint: string,
  answer: string
};

type words = array(word);

type clue = (int, word);

type clues = array(clue);

type contents =
  | Char(char)
  | Empty
  | FilledIn;

let contents_equal = fun(x: contents, y: contents) {
  switch ((x, y)) {
  | (Char(c), Char(d)) when c == d => true
  | (Empty, Empty) => true
  | (FilledIn, FilledIn) => true
  | _ => false
  };
};

type row = array(contents);

type grid = array(row);

type loadable('a, 'b) = | Loaded('a) | Loading | Error('b) | None;

let get_bounds_from_words = fun(_words: words): coords => {
  _words |> Array.fold_left(
    (currentBounds, _word) => {
      let wordBounds = {
        col: _word.start.col + switch(_word.dir) {
        | Down => 1
        | Right => String.length(_word.answer)
        },
        row: _word.start.row + switch(_word.dir) {
          | Down => String.length(_word.answer)
          | Right => 1
          },
      };
      {
        col: max(wordBounds.col, currentBounds.col),
        row: max(wordBounds.row, currentBounds.row),
      };
    },
    { col: 0, row: 0 }
  );
};


let make_empty_grid = fun(bounds: coords) => {
  Array.make(bounds.row, None) |> Array.map((_) => Array.make(bounds.col, Empty));
};

let get_grid_bounds = fun(vGrid: grid) => {
  {
    row: vGrid |> Array.length,
    col: vGrid[0] |> Array.length
  };
};

let get_from_grid = fun(vGrid: grid, vCoords: coords) => {
  vGrid[vCoords.row][vCoords.col];
};

let set_in_grid = fun(vGrid: grid, vCoords: coords, value: contents) => {
  vGrid[vCoords.row][vCoords.col] = value;
};

let set_in_grid_safe = fun(vGrid: grid, vCoords: coords, value: contents) => {
  let currentVal = vGrid[vCoords.row][vCoords.col];
  switch (currentVal) {
  | _ when contents_equal(currentVal, value) => ()
  | Empty => vGrid[vCoords.row][vCoords.col] = value
  | _ => raise(Arg.Bad("Value is inconsistent with existing grid"))
  };
};

let set_in_grid_overwrite = fun(vGrid: grid, vCoords: coords, value: contents) => {
  let currentVal = vGrid[vCoords.row][vCoords.col];
  switch (currentVal) {
  | Char(_)
  | Empty => vGrid[vCoords.row][vCoords.col] = value
  | FilledIn => raise(Arg.Bad("Value is inconsistent with existing grid"))
  };
};

let calc_grid_position = fun(start: coords, dir: direction, index: int) => {
  switch (dir) {
  | Down => { col: start.col, row: start.row + index }
  | Right => { col: start.col + index, row: start.row }
  };
};

let build_full_grid = fun(vWords: words): grid => {
  let vGrid = get_bounds_from_words(vWords) |> make_empty_grid;
  vWords |> Array.iter(
    (vWord) => {
      vWord.answer |> String.iteri((ind, letter) => {
        let letterCoords = calc_grid_position(vWord.start, vWord.dir, ind);
        set_in_grid_safe(vGrid, letterCoords, Char(letter));
      });
    }
  );
  vGrid |> Array.map((vRow) => {
    vRow |> Array.map((vContents) => {
      switch (vContents) {
      | Char(c) => Char(c)
      | _ => FilledIn
      };
    });
  });
};

let empty_grid =fun(vGrid: grid): grid => {
  let newGrid = vGrid |> get_grid_bounds |> make_empty_grid;
  vGrid |> Array.iteri((rowNum, vRow) => {
    vRow |> Array.iteri((colNum, vContents) => {
      switch (vContents) {
      | FilledIn => newGrid[colNum][rowNum] = FilledIn
      | _ => newGrid[colNum][rowNum] = Empty
      }
    });
  });
  newGrid;
};

let rec compareWords = fun(comparators: list((word, word) => int), wordA: word, wordB: word): int {
  switch (comparators) {
    | [] => 0
    | [comparator, ...rest] => {
      let result = comparator(wordA, wordB);
      switch (result) {
        | 0 => compareWords(rest, wordA, wordB)
        | x => x
      }
    }
  }
};

let make_clues = fun(vWords: words): clues => {
  let newWords = Array.copy(vWords);
  Array.sort(compareWords([
    (wordA, wordB) => (wordA.start.row - wordB.start.row),
    (wordA, wordB) => (wordA.start.col - wordB.start.col),
    (wordA, wordB) => switch (wordA.dir, wordB.dir) {
      | (Right, Down) => -1
      | (Down, Right) => 1
      | _ => 0
    }
  ]), newWords);

  let (_, _, vClues) = Array.fold_left((memo, vWord) => {
    let (clueNum, lastCoords, vClues) = memo;
    switch ((vWord.start.row - lastCoords.row, vWord.start.col - lastCoords.col)) {
    | (0, 0) => (clueNum, lastCoords, Array.append(vClues, [|(clueNum, vWord)|]))
    | _ => (clueNum + 1, vWord.start, Array.append(vClues, [|(clueNum + 1, vWord)|]))
    }
  },
    (0, { row: -1, col: -1 }, [||]: clues),
    newWords
  );

  vClues;
};

let rec array_find = fun(test: ('a) => bool, arr: array('a)): option('a) => {
  let arrayLength = arr |> Array.length;
  let thisEl = Array.get(arr, 0);
  let rest = Array.sub(arr, 1, arrayLength - 1);
  switch (thisEl |> test) {
    | true => Some(thisEl)
    | false => {
      switch arrayLength {
      | 1 => None
      | _ => array_find(test, rest)
      };
    };
  };
};
