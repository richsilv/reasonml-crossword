let component = ReasonReact.statelessComponent("Cell");

let make = (~value: Utils.contents, _children) => {
  ...component,
  render: (_self) => <div> {
    switch (value) {
      | Empty => <div className="cell" />
      | FilledIn => <div className="cell filled-in" />
      | Char(_value) => <div className="cell"> { ReasonReact.stringToElement(String.make(1, _value)) } </div>
    }
  } </div>
};
