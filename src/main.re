/* let testGrid = [|
[|Utils.Char('F'), Utils.Char('O'), Utils.Char('O')|],
[|Utils.FilledIn, Utils.Char('L'), Utils.FilledIn|],
[|Utils.Char('B'), Utils.Char('A'), Utils.Empty|],
|]; */

let vWords: Utils.words = [|
  { start: { row: 0, col: 0 }, dir: Utils.Right, hint: "Ryhmes with poo", answer: "FOO" },
  { start: { row: 0, col: 1 }, dir: Utils.Down, hint: "Hello in Spanish", answer: "OLA" },
  { start: { row: 2, col: 0 }, dir: Utils.Right, hint: "Good place for a drink", answer: "BAR" }
|];

let vGrid = vWords |> Utils.build_full_grid;

ReactDOMRe.renderToElementWithId(<App />, "app");
