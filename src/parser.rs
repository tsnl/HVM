// This parse library is more high-level and functional than existing alternatives.
// A Parser is defied as (with details omitted):
//
//   Answer<A> = Result<(State, A), String>
//   Parser<A> = Fn(State) -> Answer<A>>
//
// Similarly to https://github.com/AndrasKovacs/flatparse, there are 2 ways to fail.
//
// 1. Recoverable. Use Parser<Option<A>>, and return:
//    - Ok((new_state, Some(result))) if it succeeds
//    - Ok((old_state, None))         if it fails
//
//    This backtracks, and should be used to implement alternatives. For example, if you're
//    parsing an AST, "Animal", with 2 constructors, dog and cat, then you should implement:
//
//    parse_dog    : Parser<Option<Animal>>
//    parse_cat    : Parser<Option<Animal>>
//    parse_animal : Parser<Animal>
//
// 2. Irrecoverable. Return:
//    - Err(error_message)
//
//    This will abort the entire parser, like a "throw", and return the error message. Use this
//    when you know that only one parsing branch can reach this location, yet the source is wrong.
//
// Check the Testree example at the bottom of this file.

#![allow(dead_code)]

// Types
// =====

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct State<'a> {
  pub code: &'a str,
  pub index: usize,
}

impl<'a> State<'a> {
  fn rest(&self) -> &'a str {
    &self.code[self.index..]
  }
}

pub type Answer<'a, A> = Result<(State<'a>, A), String>;
pub type Parser<'a, A> = Box<dyn Fn(State<'a>) -> Answer<'a, A>>;

// Utils
// =====

pub fn equal_at(text: &str, test: &str, i: usize) -> bool {
  return &text.as_bytes()[i..std::cmp::min(text.len(), i + test.len())] == test.as_bytes();
}

pub fn flatten(texts: &[&str]) -> String {
  texts.concat()
}

pub fn lines(text: &str) -> Vec<String> {
  text.lines().map(String::from).collect()
}

pub fn find(text: &str, target: &str) -> usize {
  text.find(target).unwrap()
}

pub fn read<'a, A>(parser: Parser<'a, A>, code: &'a str) -> A {
  match parser(State { code, index: 0 }) {
    Ok((_state, value)) => value,
    Err(msg) => {
      println!("{}", msg);
      panic!("No parse.");
    }
  }
}

// Elims
// =====

pub fn head(state: State) -> Option<char> {
  return state.code[state.index..].chars().next();
}

pub fn head_default(state: State) -> char {
  if let Some(got) = head(state) {
    got
  } else {
    '\0'
  }
}

pub fn tail(state: State) -> State {
  let _fst = head(state);
  let add = match head(state) {
    Some(c) => c.len_utf8(),
    None => 0,
  };
  State { code: state.code, index: state.index + add }
}

pub fn get_char(state: State) -> Answer<char> {
  let (state, _skipped) = skip(state)?;
  if let Some(got) = head(state) {
    let state = State { code: state.code, index: state.index + got.len_utf8() };
    Ok((state, got))
  } else {
    Ok((state, '\0'))
  }
}

pub fn get_char_parser<'a>() -> Parser<'a, char> {
  Box::new(get_char)
}

// Skippers
// ========

pub fn skip_comment(state: State) -> Answer<bool> {
  let mut state = state;
  if state.index + 1 < state.code.len() && equal_at(state.code, "//", state.index) {
    state.index += 2;
    while state.index < state.code.len() && !equal_at(state.code, "\n", state.index) {
      state.index += 1;
    }
    Ok((state, true))
  } else {
    Ok((state, false))
  }
}

pub fn skip_comment_parser<'a>() -> Parser<'a, bool> {
  Box::new(skip_comment)
}

pub fn skip_spaces(state: State) -> Answer<bool> {
  pub fn is_space(chr: char) -> bool {
    chr == ' ' || chr == '\n' || chr == '\t' || chr == '\r'
  }
  let mut state = state;
  if state.index < state.code.len() && is_space(head_default(state)) {
    state.index += 1;
    while state.index < state.code.len() && is_space(head_default(state)) {
      state.index += 1;
    }
    Ok((state, true))
  } else {
    Ok((state, false))
  }
}

pub fn skip_spaces_parser<'a>() -> Parser<'a, bool> {
  Box::new(skip_spaces)
}

pub fn skip(state: State) -> Answer<bool> {
  let (state, comment) = skip_comment(state)?;
  let (state, spaces) = skip_spaces(state)?;
  if comment || spaces {
    let (state, _skipped) = skip(state)?;
    Ok((state, true))
  } else {
    Ok((state, false))
  }
}

pub fn skip_parser<'a>() -> Parser<'a, bool> {
  Box::new(skip)
}

// Strings
// =======

// Attempts to match a string right after the cursor.
// Returns true if successful. Consumes string.
pub fn text_here<'a>(pat: &str, state: State<'a>) -> Answer<'a, bool> {
  if equal_at(state.code, pat, state.index) {
    let state = State { code: state.code, index: state.index + pat.len() };
    Ok((state, true))
  } else {
    Ok((state, false))
  }
}

pub fn text_here_parser<'a>(pat: &'static str) -> Parser<'a, bool> {
  Box::new(move |x| text_here(pat, x))
}

// Like 'text_here', but skipping spaces and comments before.
pub fn text<'a>(pat: &str, state: State<'a>) -> Answer<'a, bool> {
  let (state, _skipped) = skip(state)?;
  let (state, matched) = text_here(pat, state)?;
  Ok((state, matched))
}

pub fn text_parser<'a>(pat: &'static str) -> Parser<'a, bool> {
  Box::new(move |x| text(pat, x))
}

// Like 'text', but aborts if there is no match.
pub fn consume<'a>(pat: &str, state: State<'a>) -> Answer<'a, ()> {
  let (state, matched) = text(pat, state)?;
  if matched {
    Ok((state, ()))
  } else {
    expected(pat, pat.len(), state)
  }
}

pub fn consume_parser<'a>(pat: &'static str) -> Parser<'a, ()> {
  Box::new(move |x| consume(pat, x))
}

// Returns true if we are at the end of the file, skipping spaces and comments.
pub fn done(state: State) -> Answer<bool> {
  let (state, _skipped) = skip(state)?;
  Ok((state, state.index == state.code.len()))
}

pub fn done_parser<'a>() -> Parser<'a, bool> {
  Box::new(done)
}

// Blocks
// ======

// Checks if a dry-run of the first parser returns true.
// If so, applies the second parser, returning Some.
// If no, return None.
pub fn guard<'a, A: 'a>(
  head: Parser<'a, bool>,
  body: Parser<'a, A>,
  state: State<'a>,
) -> Answer<'a, Option<A>> {
  let (state, _skipped) = skip(state)?;
  let (_, matched) = head(state)?;
  if matched {
    let (state, got) = body(state)?;
    Ok((state, Some(got)))
  } else {
    Ok((state, None))
  }
}

// Applies optional parsers in sequence.
// Returns the first that succeeds.
// If none succeeds, aborts.
pub fn grammar<'a, A: 'a>(
  name: &'static str,
  choices: &[Parser<'a, Option<A>>],
  state: State<'a>,
) -> Answer<'a, A> {
  for choice in choices {
    let (state, result) = choice(state)?;
    if let Some(value) = result {
      return Ok((state, value));
    }
  }
  expected(name, 1, state)
}

// Combinators
// ===========

// Evaluates a parser and returns its result, but reverts its effect.
pub fn dry<'a, A: 'a>(parser: Parser<'a, A>, state: State<'a>) -> Answer<'a, A> {
  let (_new_state, result) = parser(state)?;
  Ok((state, result))
}

// Evaluates a parser until a condition is met. Returns an array of results.
pub fn until<'a, A: 'a>(
  delim: Parser<'a, bool>,
  parser: Parser<'a, A>,
  state: State<'a>,
) -> Answer<'a, Vec<A>> {
  let mut state = state;
  let mut result = Vec::new();
  loop {
    let (new_state, delimited) = delim(state)?;
    if delimited {
      state = new_state;
      break;
    } else {
      let (new_state, a) = parser(new_state)?;
      state = new_state;
      result.push(a);
    }
  }
  Ok((state, result))
}

// Evaluates a list-like parser, with an opener, separator, and closer.
pub fn list<'a, A: 'a, B: 'a>(
  parse_open: Parser<'a, bool>,
  parse_sep: Parser<'a, bool>,
  parse_close: Parser<'a, bool>,
  parse_elem: Parser<'a, A>,
  make: Box<dyn Fn(Vec<A>) -> B>,
  state: State<'a>,
) -> Answer<'a, B> {
  let (state, _) = parse_open(state)?;
  let mut state = state;
  let mut elems = Vec::new();
  loop {
    let (new_state, done) = parse_close(state)?;
    let (new_state, _) = parse_sep(new_state)?;
    if done {
      state = new_state;
      break;
    } else {
      let (new_state, elem) = parse_elem(new_state)?;
      state = new_state;
      elems.push(elem);
    }
  }
  Ok((state, make(elems)))
}

// Name
// ====

// Is this character a valid name letter?
fn is_letter(chr: char) -> bool {
  ('A'..='Z').contains(&chr)
    || ('a'..='z').contains(&chr)
    || ('0'..='9').contains(&chr)
    || chr == '_'
    || chr == '.'
}

// Parses a name right after the parsing cursor.
pub fn name_here(state: State) -> Answer<String> {
  let mut name: String = String::new();
  let mut state = state;
  while let Some(got) = head(state) {
    if is_letter(got) {
      name.push(got);
      state = tail(state);
    } else {
      break;
    }
  }
  Ok((state, name))
}

// Parses a name after skipping.
pub fn name(state: State) -> Answer<String> {
  let (state, _skipped) = skip(state)?;
  name_here(state)
}

// Parses a non-empty name after skipping.
pub fn name1(state: State) -> Answer<String> {
  let (state, name1) = name(state)?;
  if !name1.is_empty() {
    Ok((state, name1))
  } else {
    expected("name", 1, state)
  }
}

// Errors
// ======

pub fn expected<'a, A>(name: &str, size: usize, state: State<'a>) -> Answer<'a, A> {
  return Err(format!(
    "Expected {}:\n{}",
    name,
    &highlight(state.index, state.index + size, state.code)
  ));
}

pub fn highlight(from_index: usize, to_index: usize, code: &str) -> String {
  //let open = "<<<<####";
  //let close = "####>>>>";
  let open = "««««";
  let close = "»»»»";
  let open_color = "\x1b[4m\x1b[31m";
  let close_color = "\x1b[0m";
  let mut from_line = 0;
  let mut to_line = 0;
  for (i, c) in code.chars().enumerate() {
    if c == '\n' {
      if i < from_index {
        from_line += 1;
      }
      if i < to_index {
        to_line += 1;
      }
    }
  }
  let code: String = flatten(&[
    &code[0..from_index],
    open,
    &code[from_index..to_index],
    close,
    &code[to_index..code.len()],
  ]);
  let lines: Vec<String> = lines(&code);
  let block_from_line = std::cmp::max(from_line as i64 - 3, 0) as usize;
  let block_to_line = std::cmp::min(to_line + 3, lines.len());
  let mut text = String::new();
  for (i, line) in lines[block_from_line..block_to_line].iter().enumerate() {
    let numb = block_from_line + i;
    let rest;
    if numb == from_line && numb == to_line {
      rest = flatten(&[
        &line[0..find(line, open)],
        open_color,
        &line[find(line, open) + open.len()..find(line, close)],
        close_color,
        &line[find(line, close) + close.len()..line.len()],
        "\n",
      ]);
    } else if numb == from_line {
      rest = flatten(&[
        &line[0..find(line, open)],
        open_color,
        &line[find(line, open) + open.len()..line.len()],
        "\n",
      ]);
    } else if numb > from_line && numb < to_line {
      rest = flatten(&[open_color, line, close_color, "\n"]);
    } else if numb == to_line {
      rest = flatten(&[
        &line[0..find(line, close)],
        close_color,
        &line[find(line, close) + close.len()..line.len()],
        "\n",
      ]);
    } else {
      rest = flatten(&[line, "\n"]);
    }
    let line = format!("    {} | {}", numb, rest);
    text.push_str(&line);
  }
  text
}

// Tests
// =====

pub enum Testree {
  Node { lft: Box<Testree>, rgt: Box<Testree> },
  Leaf { val: String },
}

pub fn testree_show(tt: &Testree) -> String {
  match tt {
    Testree::Node { lft, rgt } => format!("({} {})", testree_show(lft), testree_show(rgt)),
    Testree::Leaf { val } => val.to_string(),
  }
}

pub fn node_parser<'a>() -> Parser<'a, Option<Box<Testree>>> {
  Box::new(|state| {
    guard(
      text_parser("("),
      Box::new(|state| {
        let (state, _) = consume("(", state)?;
        let (state, lft) = testree_parser()(state)?;
        let (state, rgt) = testree_parser()(state)?;
        let (state, _) = consume(")", state)?;
        Ok((state, Box::new(Testree::Node { lft, rgt })))
      }),
      state,
    )
  })
}

pub fn leaf_parser<'a>() -> Parser<'a, Option<Box<Testree>>> {
  Box::new(|state| {
    guard(
      text_parser(""),
      Box::new(|state| {
        let (state, val) = name(state)?;
        Ok((state, Box::new(Testree::Leaf { val })))
      }),
      state,
    )
  })
}

pub fn testree_parser<'a>() -> Parser<'a, Box<Testree>> {
  Box::new(|state| {
    let (state, tree) = grammar("Testree", &[node_parser(), leaf_parser()], state)?;
    Ok((state, tree))
  })
}

#[cfg(test)]
mod tests {
  use super::*;
  
  fn mocked_state_1<'a>(index: usize) -> State<'a> {
    State {
      code: "foo bar baz",
      index
    }
  }

  fn mocked_state_2<'a>(index: usize) -> State<'a> {
    State {
      code: "// Compose functions
        // Computes f^(2^n)
        (Comp 0 f x) = (f x)
        (Comp n f x) = (Comp (- n 1) λk(f (f k)) x)
      ",
      index
    }
  }

  fn parse_test_ok<'a, A: PartialEq + std::fmt::Debug>(
    parser: Parser<'a, A>, 
    expected_result: A, 
    state_before: State<'a>, 
    state_after: State<'a>
  ) {
    let state = state_before;
    let parsed = parser(state);
    assert!(parsed.is_ok());
    if let Ok((state, result)) = parsed {
      assert_eq!(expected_result, result);
      assert_eq!(state_after, state);
    }
  }

  fn parse_test_err<'a, A: PartialEq + std::fmt::Debug>(
    parser: Parser<'a, A>,
    state_before: State<'a>
  ) {
    let state = state_before;
    let parsed = parser(state);
    assert!(parsed.is_err());
  }


  // gen fake parsers used only in tests
  fn mocked_parser_gen<'a>(text: &'static str) -> Parser<'a, bool> {
    Box::new(move |state: State| -> Answer<bool> {
      let pos = state.index;
      if &state.code[pos..pos+3] == text {
        let state = State {
          code: state.code,
          index: state.index+3
        };
        Ok((state, true))
      } else {
        Err(format!("not a {}", text))
      }
    })
  }

  // tests for the mocked parser
  #[test]
  fn mocked_parser_gen_test() {
    parse_test_ok(mocked_parser_gen("foo"), true, mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(mocked_parser_gen("bar"), true, mocked_state_1(4), mocked_state_1(7));
  }

  #[test]
  fn mocked_parser_fail() {
    parse_test_err(mocked_parser_gen("foo"), mocked_state_1(1));
    parse_test_err(mocked_parser_gen("foo"), mocked_state_1(2));
  }

  // parses a 'foo' and update index
  fn mocked_parser_foo<'a>() -> Parser<'a, bool> {
    mocked_parser_gen("foo")
  }

  // parses a 'bar' and update index
  fn mocked_parser_bar<'a>() -> Parser<'a, bool> {
    mocked_parser_gen("bar")
  }

  // parses a 'baz' and update index
  fn mocked_parser_baz<'a>() -> Parser<'a, bool> {
    mocked_parser_gen("baz")
  }

  // Utils
  #[test]
  fn equal_at_test() {
    assert!(equal_at("foo bar baz", "foo", 0));
    assert!(equal_at("foo bar baz", "bar", 4));
    assert!(equal_at("foo bar baz", "baz", 8));
    assert!(!equal_at("foo bar baz", "bar", 0));
  }

  #[test]
  #[should_panic]
  fn equal_at_panic() {
    equal_at("", "baz", 8);
  }

  #[test]
  fn flatten_test() {
    assert_eq!(
      "foobarbaz",
      flatten(&["foo", "bar", "baz"])
    );
  }
  
  #[test]
  fn lines_test() {
    assert_eq!(
      vec!["foo", "bar", "baz"],
      lines("foo\nbar\nbaz")
    );
    assert_eq!(
      vec!["foobarbaz"],
      lines("foobarbaz")
    );
    let empty : Vec<String> = vec![];
    assert_eq!(
      empty,
      lines("")
    );
  }

  #[test]
  fn find_test() {
    assert_eq!(8, find("foo bar baz", "baz"));
    assert_eq!(4, find("foo bar baz", "bar"));
  }

  #[test]
  #[should_panic]
  fn find_panic() {
    find("foo bar", "baz");
  }
    
  #[test]
  fn read_test() {
    let parser = mocked_parser_foo();
    assert!(read(parser, "foo bar baz"));
  }

  #[test]
  #[should_panic]
  fn read_fail() {
    let parser = mocked_parser_foo();
    read(parser, "bar foo");
  }
  
  // Elims
  // =====

  #[test]
  fn head_test() {
    assert_eq!(head(mocked_state_1(0)), Some('f'));
    assert_eq!(head(mocked_state_1(4)), Some('b'));
    assert_eq!(head(mocked_state_1(5)), Some('a'));
    assert_eq!(head(mocked_state_1(11)), None);
  }

  #[test]
  #[should_panic]
  fn head_panic() {
    head(mocked_state_1(12));
  }

  #[test]
  fn head_default_test() {
    assert_eq!(head_default(mocked_state_1(0)), 'f');
    assert_eq!(head_default(mocked_state_1(4)), 'b');
    assert_eq!(head_default(mocked_state_1(5)), 'a');
    assert_eq!(head_default(mocked_state_1(11)), '\0');
  }

  #[test]
  #[should_panic]
  fn head_default_panic() {
    head_default(mocked_state_1(12));
  }

  #[test]
  fn tail_test() {
    assert_eq!(tail(mocked_state_1(0)), mocked_state_1(1));
    assert_eq!(tail(mocked_state_1(4)), mocked_state_1(5));
    assert_eq!(tail(mocked_state_1(5)), mocked_state_1(6));
    assert_eq!(tail(mocked_state_1(11)), mocked_state_1(11));
  }

  #[test]
  #[should_panic]
  fn tail_panic() {
    tail(mocked_state_1(12));
  }

  #[test]
  fn get_char_parser_test() {
    parse_test_ok(get_char_parser(), 'b', mocked_state_1(4), mocked_state_1(5));
    parse_test_ok(get_char_parser(), '\0', mocked_state_1(11), mocked_state_1(11));
  }

  // Skippers
  // ========

  #[test]
  fn skip_comment_parser_test() {
    parse_test_ok(skip_comment_parser(), false, mocked_state_1(0), mocked_state_1(0));
    parse_test_ok(skip_comment_parser(), true, mocked_state_2(0), mocked_state_2(20));
  }

  #[test]
  fn skip_spaces_parser_test() {
    parse_test_ok(skip_spaces_parser(), false, mocked_state_1(0), mocked_state_1(0));
    parse_test_ok(skip_spaces_parser(), true, mocked_state_1(3), mocked_state_1(4));
    parse_test_ok(skip_spaces_parser(), true, mocked_state_2(20), mocked_state_2(29));
  }

  #[test]
  fn skip_parser_test() {
    parse_test_ok(skip_parser(), false, mocked_state_1(0), mocked_state_1(0));
    parse_test_ok(skip_parser(), true, mocked_state_1(3), mocked_state_1(4));
    parse_test_ok(skip_parser(), true, mocked_state_2(0), mocked_state_2(57));
  }

  // Strings
  // =======

  #[test]
  fn text_here_parser_test() {
    parse_test_ok(text_here_parser("foo"), true, mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(text_here_parser("foo"), false, mocked_state_1(4), mocked_state_1(4));
    parse_test_ok(text_here_parser("bar"), true, mocked_state_1(4), mocked_state_1(7));
  }

  #[test]
  fn text_parser_test() {
    parse_test_ok(text_parser("foo"), true, mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(text_parser("foo"), false, mocked_state_1(4), mocked_state_1(4));
    parse_test_ok(text_parser("("), true, mocked_state_2(0), mocked_state_2(58));
    parse_test_ok(text_parser("foo"), false, mocked_state_2(0), mocked_state_2(57));
  }

  #[test]
  fn consume_parser_test_ok() {
    parse_test_ok(consume_parser("foo"), (), mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(consume_parser("("), (), mocked_state_2(0), mocked_state_2(58));
  }

  #[test]
  fn consume_parser_test_err() {
    parse_test_err(consume_parser("foo"), mocked_state_1(4));
    parse_test_err(consume_parser("foo"), mocked_state_2(0));
  }

  #[test]
  fn done_parser_test() {
    parse_test_ok(done_parser(), true, mocked_state_1(11), mocked_state_1(11));
    parse_test_ok(done_parser(), false, mocked_state_1(10), mocked_state_1(10));
    parse_test_ok(done_parser(), true, mocked_state_2(130), mocked_state_2(137));
  }

  #[test]
  fn guard_test() {
    // created only to use the function below
    let guard_parser = |first_text, second_text| { 
      Box::new(move |x| { 
        guard(text_parser(first_text), text_parser(second_text), x)
      })
    };
    parse_test_ok(guard_parser("foo", "foo bar"), Some(true), mocked_state_1(0), mocked_state_1(7));
    parse_test_ok(guard_parser("foo", "bar"), Some(false), mocked_state_1(0), mocked_state_1(0));
    parse_test_ok(guard_parser("bar", "bar baz"), None, mocked_state_1(0), mocked_state_1(0));
  }

  fn grammar_test() {
    todo!()
    // created only to use the function below
    // let grammar_parser = |texts: Vec<&str>| { 
    //   Box::new(move |x| { 
    //     let parsers = texts.iter().map(|&text| text_parser(text)).collect();
    //     grammar("test", parsers, x)
    //   })
    // };
  }


  // Combinators
  // ===========
  #[test]
  fn dry_test() {
    assert_eq!(dry(mocked_parser_foo(), mocked_state_1(0)), Ok((mocked_state_1(0), true)));
    assert_eq!(dry(mocked_parser_foo(), mocked_state_1(4)), Err("not a foo".to_string()));
    assert_eq!(dry(text_parser("bar"),  mocked_state_1(7)), Ok((mocked_state_1(7), false)));
  }

  #[test]
  fn until_test() {
    assert_eq!(
      until(skip_spaces_parser(), get_char_parser(), mocked_state_1(0)), 
      Ok((mocked_state_1(4), vec!['f', 'o', 'o']))
    );
    assert_eq!(
      until(skip_spaces_parser(), get_char_parser(), mocked_state_1(4)), 
      Ok((mocked_state_1(8), vec!['b', 'a', 'r']))
    );
    assert_eq!(
      until(skip_spaces_parser(), get_char_parser(), mocked_state_1(3)), 
      Ok((mocked_state_1(4), vec![]))
    );
    assert_eq!(
      until(text_parser("bar"), get_char_parser(), mocked_state_1(0)), 
      Ok((mocked_state_1(7), vec!['f', 'o', 'o']))
    );
  }

  #[test]
  fn list_test() {
    let result = 
      list(
        text_parser("foo"), 
        skip_spaces_parser(), 
        text_parser("baz"),
        get_char_parser(), 
        Box::new(|x| x.into_iter().collect()), 
        mocked_state_1(0)
    );
    assert_eq!(
      result,
      Ok((mocked_state_1(11), "bar".to_string()))
    )
  }

  // Name
  // ====

  #[test]
  fn is_letter_test() {
    assert!(is_letter('C'));
    assert!(is_letter('k'));
    assert!(is_letter('4'));
    assert!(is_letter('_'));
    assert!(is_letter('.'));
    assert!(!is_letter('?'));
    assert!(!is_letter('/'));
    assert!(!is_letter('('));
    assert!(!is_letter('='));
  }

  #[test]
  fn name_here_test() {
    parse_test_ok(Box::new(name_here), "foo".to_string(), mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(Box::new(name_here), "bar".to_string(), mocked_state_1(4), mocked_state_1(7));
    parse_test_ok(Box::new(name_here), "".to_string(), mocked_state_1(3), mocked_state_1(3));
  }

  #[test]
  fn name_test() {
    parse_test_ok(Box::new(name), "foo".to_string(), mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(Box::new(name), "bar".to_string(), mocked_state_1(4), mocked_state_1(7));
    parse_test_ok(Box::new(name), "bar".to_string(), mocked_state_1(3), mocked_state_1(7));
  }
  
  #[test]
  fn name1_test() {
    parse_test_ok(Box::new(name1), "foo".to_string(), mocked_state_1(0), mocked_state_1(3));
    parse_test_ok(Box::new(name1), "bar".to_string(), mocked_state_1(4), mocked_state_1(7));
    parse_test_ok(Box::new(name1), "bar".to_string(), mocked_state_1(3), mocked_state_1(7));
  }

  // Errors
  // ====

  #[test]
  fn highlight_test() {
    let open_color = "\x1b[4m\x1b[31m";
    let close_color = "\x1b[0m";
    assert_eq!(
      highlight(3, 8, "foo bar baz"),
      format!("    0 | foo{} bar {}baz\n", open_color, close_color)
    );
    assert_eq!(
      highlight(3, 10, "foo\n bar \n baz"),
      format!("    0 | foo{}\n    1 | {} bar {}\n    2 | {} baz\n", open_color, open_color, close_color, close_color)
    );
    assert_eq!(
      highlight(3, 6, "foo\n bar \n baz"),
      format!("    0 | foo{}\n    1 |  b{}ar \n    2 |  baz\n", open_color, close_color)
    );
  }

  #[test]
  fn expected_test() {
    let init = 0;
    let size = 3;
    let expected1: Answer<()> = expected("vasco", size, mocked_state_1(init));
    assert_eq!(
      expected1,
      Err(format!("Expected {}:\n{}", "vasco", highlight(init, size, mocked_state_1(0).code)))
    );
  }
}
