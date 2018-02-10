import React from 'react';
import ProjectTemplate from './_project';
import CodeTemplate from './_code';
import { connect } from 'react-redux';
import { Card } from 'antd';

function __getBlobPathFromHash () {
    let treeUriPosition = window.location.hash.indexOf ('/blob');
    let path = window.location.hash.substring (treeUriPosition + '/blob'.length);
    return path;
}

class Page extends ProjectTemplate {
    state = { initialized: false }
    
    treeItemOnClick (record) {
        this.setState({ initialized: false }, () => this.props.dispatch({
            type: 'tree/enterSubTree',
            payload: {
                ...record,
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName: this.props.match.params.branchName
            },
            after: () => this.setState({ initialized: true })
        }));
    }

    renderPath() {
        return <div style={{ overflow: 'hidden', textOverflow: 'ellipsis' }}>{
                this.props.pathItems.map((item, index) => <span style={{ fontSize: 18 }} key={ index }>
                { index === 0 ? '' : '/' }
                <span style={{ margin: '0 4px' }}>
                    {
                        index === this.props.pathItems.length - 1 && index !== 0
                        ? <span style={{ color: '#00000' }}>{ item.name }</span>
                        : <a
                            style={{ color: '#0366d6' }}
                            onClick={() => this.treeItemOnClick ({ path: item.path, type: 'Tree' }) }>
                            { item.name }
                        </a>
                    }
                </span>
            </span>)}
        </div>
    }
    
    unit() {
        return <div>
            <Card title={ this.renderPath () } bodyStyle={{ paddingTop: '15px', paddingBottom: '15px' }}></Card>
                <CodeTemplate
                    dispatch={ this.props.dispatch }
                    repositoriesName={ this.props.match.params.repositoriesName }
                    repositoryName={ this.props.match.params.repositoryName }
                    branchName={ this.props.match.params.branchName }
                    path={ __getBlobPathFromHash () }
                    name={ this.props.name }
                    content={ this.props.content }
                />
        </div>
    }
}

export default connect(
    ({ blob, branches }) => ({
        entryUnit: 'repositories',
        projectEntryUnit: 'code',
        content: blob.content,
        path: blob.path,
        name: blob.name,
        pathItems: blob.pathItems,
        branches: branches.branches
    })
)(Page);