(defun closure-test (y)
  (let ((beep y))
    #'(lambda (x)
        (incf beep)
        (+ x beep))))


