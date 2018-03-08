let component = ReasonReact.statelessComponent("Grid");

let make = (~grid: Utils.grid, ~words: Utils.words, _children) => {
  ...component,
  render: (_self) => {
    <div className="grid"> {
      Utils.fold_left_index(
        (array, row, rowIndex) => {
          Array.concat([
            array,
            Array.mapi(
              (colIndex, cellValue) =>
                <Cell key={string_of_int(rowIndex) ++ "-" ++ string_of_int(colIndex)} value={cellValue} />,
              row
            )
          ]);
        },
        [||],
        grid
      ) |> ReasonReact.arrayToElement;
    }
    </div>
  }
};
