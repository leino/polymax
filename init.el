(require 'compile)

(setq init-script-path (convert-standard-filename (file-name-directory load-file-name)))
(setq project-root-path init-script-path)
(setq execution-root-path (concat project-root-path "builds"))
(setq build-output-path (concat project-root-path "builds"))
(setq source-path (concat project-root-path "source"))

(setq vneck-executable-path (concat build-output-path "\\" "vneck_test.exe"))
(setq vneck-build-script-path (concat project-root-path "build.bat"))
(setq vneck-build-command (concat vneck-build-script-path " " source-path " " build-output-path))
(setq vneck-run-command (concat "cd " execution-root-path " && " vneck-executable-path "&"))

(setq test-executable-path (concat build-output-path "\\" "buffer_test.exe"))
(setq test-build-script-path (concat project-root-path "build_tests.bat"))
(setq test-build-command (concat test-build-script-path " " source-path " " build-output-path))
(setq test-run-command (concat "cd " execution-root-path " && " test-executable-path "&"))

;; saves everything and recompiles
(global-set-key (kbd "<f9>")
                (lambda ()
                  (interactive)
                  (setq compile-command vneck-build-command)
                  (setq compilation-finish-functions nil)
                  (save-some-buffers 1) ; save all buffers with changes
                  (recompile)))

;; saves, rebuilds and executes the program
(global-set-key (kbd "<f10>")
		(lambda ()
		  (interactive)
          (setq compile-command vneck-build-command)
          (save-some-buffers 1) ; save all buffers with changes
          (setq compilation-finish-functions '((lambda (a b)
                                                 (interactive)
                                                 (shell-command vneck-run-command))))
          (recompile)))

;; executes the program
(global-set-key (kbd "<f5>")
		(lambda ()
		  (interactive)
		  (shell-command vneck-run-command)))



;; saves everything and recompiles the tests
(global-set-key (kbd "<f12>")
                (lambda ()
                  (interactive)
                  (setq compile-command test-build-command)
                  (setq compilation-finish-functions nil)
                  (save-some-buffers 1) ; save all buffers with changes
                  (recompile)))

;; executes the tests
(global-set-key (kbd "<f8>")
		(lambda ()
		  (interactive)
		  (shell-command test-run-command)))
