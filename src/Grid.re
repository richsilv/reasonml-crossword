let component = ReasonReact.statelessComponent("Grid");

let make = (~vGrid: Utils.grid, ~vClues: Utils.clues, _children) => {
  ...component,
  render: (_self) => {
    <div className="grid"> {
      Utils.fold_left_index(
        (array, row, rowIndex) => {
          Array.concat([
            array,
            Array.mapi(
              (colIndex, cellValue) => {
                let thisClue = vClues |> Utils.array_find((c: Utils.clue) => {
                  let (_, vWord) = c;
                  vWord.start.row === rowIndex && vWord.start.col === colIndex;
                });
                <Cell
                  key={string_of_int(rowIndex) ++ "-" ++ string_of_int(colIndex)}
                  value={cellValue} vClue={thisClue} />
              },
              row
            )
          ]);
        },
        [||],
        vGrid
      ) |> ReasonReact.arrayToElement;
    }
    </div>
  }
};
