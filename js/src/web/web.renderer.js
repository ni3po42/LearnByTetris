import { Scene, PerspectiveCamera, BoxGeometry, WebGL1Renderer, MeshBasicMaterial, Mesh, 
DirectionalLight, MeshPhongMaterial, AmbientLight } from 'three';

import { OrbitControls  } from 'three/examples/jsm/controls/OrbitControls.js'

import { rows, cols, wallMask } from '../constants.js';

export class WebRenderer {

    constructor() {


        this.buffer = new Array(rows * cols).fill(true).map(()=> ({ bCell: null, box: null}));

        this.nextPieceBuffer = new Array(4).fill(true).map(()=>({ bCell: null, box: null }) );
        this.nextPiece = null;

        this.light = new DirectionalLight(0xffffff, 1);
        this.light.position.set(0, 0, 25);
        this.light.target.position.set(0,0,0);

        this.ambientLight = new AmbientLight(0xffffff, .5);

        this.scene = new Scene();

        this.scene.add(this.light);
        this.scene.add(this.light.target);
        this.scene.add(this.ambientLight);


        this.camera = new PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.01, 500);

        this.camera.position.z = 25;
        const canvas = window.document.getElementById('canvas');
        this.renderer = new WebGL1Renderer({ canvas, antialias: true });
        this.renderer.setSize(window.innerWidth, window.innerHeight);

        this.geometry = new BoxGeometry(1,1,1);
        this.materials = [
            0x000000,
            0xff0000,
            0x00ff00,
            0xffff00,
            0x0000ff,
            0xff00ff,
            0x00ffff,
            0xffffff
        ].map(color => new MeshPhongMaterial({color}));
        
        
        const controls = new OrbitControls(this.camera, canvas);
        controls.target.set(0,5,0);
        controls.update();
    }


    init(game) {

        const that = this;
        function animate() {
            that.renderer.render(that.scene, that.camera);
            window.requestAnimationFrame(animate);
        }

        animate();
        

    }



    gameBoard(game) {

        for(const cell of game.board.scan()){

            let bufferCell = this.buffer[cell.row * cols + cell.col];

            const color = cell.data & 0x0f;

            if (color !== bufferCell.bCell){
                this.buffer[cell.row * cols + cell.col].bCell = color;

                //const isWall = !!(cell.data & wallMask);
                
                if(!color && bufferCell.box) {
                    this.scene.remove(bufferCell.box);
                    this.buffer[cell.row * cols + cell.col].box = null;
                } else if (color) {

                    if (bufferCell.box) {
                        this.scene.remove(bufferCell.box);
                        this.buffer[cell.row * cols + cell.col].box = null;
                    }

                    const box = new Mesh(this.geometry, this.materials[color]);
                    this.buffer[cell.row * cols + cell.col].box = box;
                    box.position.x = cell.col - (cols / 2);
                    box.position.y = (-cell.row) + (rows / 2);

                    this.scene.add(box);
                }
            }

        }
    
    }

    renderNextPiece(gameStatus) {
        if(this.nextPiece !== gameStatus.nextPiece){
            this.nextPiece = gameStatus.nextPiece;  
            this.nextPieceBuffer.forEach(b=>{
                this.scene.remove(b.box);
            });

            let index = 0;
            for(const cell of gameStatus.nextPiece.scan(true)) {

                const color = cell.data & 0x0f;
                const box = new Mesh(this.geometry, this.materials[color]);
                this.nextPieceBuffer[index].box = box;
                this.nextPieceBuffer[index].bCell = color;
                box.position.x = (cell.col - (cols / 2)) - 5;
                box.position.y = (-cell.row) + (rows / 2);

                this.scene.add(box);

                index++;
            }
        }
    }

    render(game, gameStatus) {

        this.gameBoard(game);
        this.renderNextPiece(gameStatus);
        
    }

}