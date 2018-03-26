type wordList = list(string);

module WordDict =
  Map.Make(
    {
      type t = int;
      let compare = compare;
    }
  );

let wordDict = WordDict.empty;
