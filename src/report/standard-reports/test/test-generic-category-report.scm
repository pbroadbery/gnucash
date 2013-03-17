(define-module (gnucash report standard-reports test test-generic-category-report))

(use-modules (ice-9 format))
(use-modules (ice-9 streams))
(use-modules (srfi srfi-1))

(use-modules (gnucash gnc-module))
(gnc:module-load "gnucash/report/report-system" 0)

(use-modules (gnucash main)) ;; FIXME: delete after we finish modularizing.
(use-modules (gnucash printf))
(use-modules (gnucash report report-system))
(use-modules (gnucash app-utils))
(use-modules (gnucash engine))
(use-modules (sw_engine))

(use-modules (gnucash report report-system streamers))
(use-modules (gnucash report report-system test test-extras))

(export run-category-report-test)

(define (set-option report page tag value)
  ((gnc:option-setter (gnc:lookup-option (gnc:report-options report)
					 page tag)) value))


(define constructor (record-constructor <report>))

;(set-option income-report gnc:pagename-general "Start Date" (cons 'relative 'start-prev-year))
;(set-option income-report gnc:pagename-general "End Date" (cons 'relative 'end-this-month))
;(set-option income-report gnc:pagename-general "Show table" #t)
;(set-option income-report gnc:pagename-general "Price Source" 'pricedb-nearest)
;(set-option income-report gnc:pagename-general "Report's currency"  (gnc-default-report-currency))

(define (run-category-report-test income-report-uuid expense-report-uuid)
  (and  (null-test income-report-uuid)
		(single-txn-test income-report-uuid)  
		(multi-acct-test expense-report-uuid)
		#t))

(define (null-test uuid)
  (let* ((income-template (gnc:find-report-template uuid))
	 (income-options (gnc:make-report-options uuid))
	 (income-report (constructor uuid "bar" income-options
				     #t #t #f #f))
	 (income-renderer (gnc:report-template-renderer income-template)))
    
    (let ((doc (income-renderer income-report)))
      (gnc:html-document-set-style-sheet! doc
					  (gnc:report-stylesheet income-report))
      (format #t "render: ~a\n" (gnc:html-document-render doc #f))
      )))


(define (single-txn-test uuid)
  (let* ((income-template (gnc:find-report-template uuid))
	 (income-options (gnc:make-report-options uuid))
	 (income-report (constructor uuid "bar" income-options
				     #t #t #f #f))
	 (income-renderer (gnc:report-template-renderer income-template)))
    (let* ((env (create-test-env))
	   (my-asset-account (env-create-root-account env ACCT-TYPE-ASSET
						      (gnc-default-report-currency)))
	   (my-expense-account (env-create-root-account env ACCT-TYPE-EXPENSE 
							(gnc-default-report-currency)))
	   (my-income-account (env-create-root-account env ACCT-TYPE-INCOME
						       (gnc-default-report-currency))))
      (env-create-daily-transactions env 
				     (gnc:get-start-this-month)
				     (gnc:get-end-this-month)
				     my-asset-account my-income-account)
      (begin
	(set-option income-report gnc:pagename-display "Show table" #t)
	(set-option income-report gnc:pagename-general "Start Date" (cons 'relative 'start-this-month))
	(set-option income-report gnc:pagename-general "End Date" (cons 'relative 'end-this-month))
	(set-option income-report gnc:pagename-general "Step Size" 'DayDelta)
	(set-option income-report gnc:pagename-general "Price Source" 'pricedb-nearest)
	(set-option income-report gnc:pagename-general "Report's currency"  (gnc-default-report-currency))
	(set-option income-report gnc:pagename-accounts "Accounts" (list my-income-account))
	(set-option income-report gnc:pagename-accounts "Show Accounts until level"  'all)
	
	(gnc:options-for-each (lambda (option)
				(format #t "Option: ~a.~a Value ~a\n" 
					(gnc:option-section option)
					(gnc:option-name option)
					(gnc:option-value option)))
			      income-options)
	
	(let ((doc (income-renderer income-report)))
	  (gnc:html-document-set-style-sheet! doc
					      (gnc:report-stylesheet income-report))
	  (let* ((result (gnc:html-document-render doc #f))
		 (tbl (stream->list
		       (pattern-streamer "<tr>" 
					 (list (list "<string> ([0-9][0-9])/([0-9][0-9])/([0-9][0-9])</td>" 1 2 3)
					       (list "<number> ([^<]*)</td>" 1))
					 result))))
	    (every (lambda (date value-list)
		     (let ((day (second date))
			   (value (first value-list)))
		       (format #t "[~a] [~a]\n" 			 
			       (string->number day) (string->number value))
		       (= (string->number day) (string->number value))))
		   (map first tbl)
		   (map second tbl))))))))

(define (list-leaves list)
  (if (not (pair? list)) 
      (cons list '())
      (fold (lambda (next acc)
	      (append (list-leaves next)
		      acc))
	    '()
	    list)))

(define (multi-acct-test expense-report-uuid)
  (let* ((expense-template (gnc:find-report-template expense-report-uuid))
	 (expense-options (gnc:make-report-options expense-report-uuid))
	 (expense-report (constructor expense-report-uuid "bar" expense-options
				     #t #t #f #f))
	 (expense-renderer (gnc:report-template-renderer expense-template)))
    (let* ((env (create-test-env))
	   (expense-accounts (env-expense-account-structure env))
	   (asset-accounts (env-create-account-structure 
			    env 	  
			    (list "Assets" 
				  (list (cons 'type ACCT-TYPE-ASSET)) 
				  (list "Bank"))))
	   (leaf-expense-accounts (list-leaves expense-accounts))
	   (bank-account (car (car (cdr asset-accounts)))))
      (format #t "Expense accounts ~a\n" leaf-expense-accounts)
      (for-each (lambda (expense-account)
		  (env-create-daily-transactions env 
						 (gnc:get-start-this-month)
						 (gnc:get-end-this-month)
						 expense-account
						 bank-account))
		leaf-expense-accounts)
      (begin
	(set-option expense-report gnc:pagename-display "Show table" #t)
	(set-option expense-report gnc:pagename-general "Start Date" (cons 'relative 'start-this-month))
	(set-option expense-report gnc:pagename-general "End Date" (cons 'relative 'end-this-month))
	(set-option expense-report gnc:pagename-general "Step Size" 'DayDelta)
	(set-option expense-report gnc:pagename-general "Price Source" 'pricedb-nearest)
	(set-option expense-report gnc:pagename-general "Report's currency"  (gnc-default-report-currency))
	(set-option expense-report gnc:pagename-accounts "Accounts" leaf-expense-accounts)
	(set-option expense-report gnc:pagename-accounts "Show Accounts until level" 2)

	(let ((doc (expense-renderer expense-report)))
	  (gnc:html-document-set-style-sheet! doc
					      (gnc:report-stylesheet expense-report))
	  (let* ((html-document (gnc:html-document-render doc #f))
		 (columns (columns-from-report-document html-document))
		 (tbl (stream->list
		       (pattern-streamer "<tr>" 
					 (list (list "<string> ([0-9][0-9])/([0-9][0-9])/([0-9][0-9])</td>" 1 2 3)
					       (list "<number> ([^<]*)</td>" 1)
					       (list "<number> ([^<]*)</td>" 1)
					       (list "<number> ([^<]*)</td>" 1)
					       (list "<number> ([^<]*)</td>" 1)
					       (list "<number> ([^<]*)</td>" 1))
					 html-document))))
	    ;(format #t "~a" html-document)
	    (and (= 6 (length columns))
		 (equal? "Date" (first columns))
		 (equal? "Auto" (second columns))
		 ;; maybe should try to check actual values
		 )))))))

(define (columns-from-report-document doc)
  (let ((columns (stream->list (pattern-streamer "<th>" 
						 (list (list "<string> ([^<]*)</" 1))
						 doc))))
    (format #t "Columns ~a\n" columns)
    (map caar columns)))

