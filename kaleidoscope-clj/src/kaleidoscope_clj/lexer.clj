(ns kaleidoscope-clj.lexer)

(defrecord Token [type value])

(defn- skip-whitespace [input]
  (drop-while #(= \space %) input))

(defn- take-while-with-rem [pred input]
  [(take-while pred input) (drop-while pred input)])

(defn- lex-keyword-or-id
  "Takes input as string and current token list. 
   Returns array with remaining input and tokens with next keyword or identifier token"
  [input tokens]
  (let [[word rest] (take-while-with-rem #(Character/isLetterOrDigit %) input)
        token (case (apply str word)
                "def" (Token. :def nil)
                "extern" (Token. :extern nil)
                :else (Token. :identifier (apply str word)))]
    [rest (conj tokens token)]))

(defn- number-char? [char]
  (or (> (Character/digit char 10) -1) (= \. char)))

(take-while-with-rem number-char? "1.0")

(defn- lex-number
  "Takes input as string and current token list. 
   Returns array with remaining input and tokens with next number token"
  [input tokens]
  (let [[num-chars rest] (take-while-with-rem number-char? input)
        num (Float/parseFloat (apply str num-chars))]
    [rest (conj tokens (Token. :number num))]))

(comment
  (skip-whitespace "   abc")
  (lex-keyword-or-id "def " [])
  (lex-number "234.94 - 2.0" []))
