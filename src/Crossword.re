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

let contents_equal = (x: contents, y: contents) =>
  switch (x, y) {
  | (Char(c), Char(d)) when c == d => true
  | (Empty, Empty) => true
  | (FilledIn, FilledIn) => true
  | _ => false
  };

type row = array(contents);

type grid = array(row);

type loadable('a, 'b) =
  | Loaded('a)
  | Loading
  | Error('b)
  | None;
